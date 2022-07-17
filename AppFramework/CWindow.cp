#include "CWindow.h"
#include "CControl.h"
#include "CApplication.h"
#include "CWindowAttachment.h"
#include "StClipSaver.h"
#include "StPortChanger.h"

#include <Events.h>
#include <Menus.h>
#include <Windows.h>
#include <ToolUtils.h>
#include "SIOUXGlobals.h"

class Control;

CWindow::CWindow(short resID, CCommandHandler *parent)
: CCommandHandler(parent ? parent : CApplication::Singleton()->GetDelegate()),
	mResID(resID), mAttachments(NULL), mControls(NULL), mWindow(NULL) {
}

CWindow::~CWindow() {
	FreeWindow();
}

void CWindow::FreeWindow() {
	CWindowAttachment *currAttachment = mAttachments;
	mAttachments = NULL;
	
	while (currAttachment) {
		CWindowAttachment *next = currAttachment->mNext;
		currAttachment->mNext = NULL;
		delete currAttachment;
		
		currAttachment = next;
	}

	CControl* currControlObject = mControls;
	mControls = NULL;
	while (currControlObject != NULL) {
		CControl *next = currControlObject->mNext;
		currControlObject->mNext = NULL;
		delete currControlObject;
		currControlObject = next;
	}

	if (mWindow) {
		DisposeWindow(mWindow);
		mWindow = NULL;
	}
}

void CWindow::CreateWindow() {
	long version = 0;
	if (Gestalt(gestaltQuickdrawVersion, &version) == noErr
		&& LoWord(version) >= gestalt8BitQD) {
		mWindow = GetNewCWindow(mResID, NULL, (WindowPtr) -1);
	} else {
		mWindow = GetNewWindow(mResID, NULL, (WindowPtr) -1);
	}
	SetWRefCon(mWindow, (long)this);
}

void CWindow::HandleMouseDown(const EventRecord &event) {
	CWindow * fw = Frontmost();
	if (fw && fw != this && fw->IsModal()) {
		SysBeep(007);
		return;
	}

	if ((event.modifiers & cmdKey) == 0) {
		SelectWindow(mWindow);
	}
	
	CWindowAttachment *currAttachment = mAttachments;
	while (currAttachment) {
		if (currAttachment->HandleMouseDown(event)) {
			return;
		}
		
		currAttachment = currAttachment->mNext;
	}

	ControlHandle clickedControl;
	Point pos = event.where;
	GlobalToLocalPoint(&pos);
	ControlPartCode part = FindControl(pos, mWindow, &clickedControl);
	if (part != kControlNoPart) {
		CControl *controlObject = (CControl*) GetCRefCon(clickedControl);
		controlObject->HandleMouseDown(event);
	}
}


void CWindow::HandleMouseUp(const EventRecord &event) {
	CWindow * fw = Frontmost();
	if (fw && fw != this && fw->IsModal()) {
		return;
	}
}

void CWindow::DrawContents() {
	long version = 0;
	if (Gestalt(gestaltQuickdrawVersion, &version) == noErr
		&& LoWord(version) >= gestalt8BitQD) {
		RGBColor backgroundColor = { 55000, 55000, 55000 };
		RGBForeColor(&backgroundColor);
	} else {
		ForeColor(whiteColor);
	}
	PaintRect(&mWindow->portRect);
	ForeColor(blackColor);
	
	UpdateControls(mWindow, mWindow->visRgn);
}

void CWindow::SetUniqueTitle(ConstStr255Param title) {
	bool foundUnique = false;
	Str255 candidateTitle;
	BlockMove(title, candidateTitle, title[0] + 1);
	int count = 1;
	while (!foundUnique) {
do_over:
		WindowPtr currWindow = FrontWindow();
		while (currWindow) {
			Str255 currTitle = {};
			GetWTitle(currWindow, currTitle);
			if (currWindow != mWindow && EqualString(currTitle, candidateTitle, true, true)) {
				++count;
				Str255 numStr;
				NumToString(count, numStr);
				BlockMove(title, candidateTitle, title[0] + 1);
				candidateTitle[0] += 1;
				candidateTitle[candidateTitle[0]] = ' ';
				BlockMoveData(numStr + 1, candidateTitle + candidateTitle[0] + 1, numStr[0]);
				candidateTitle[0] += numStr[0];
				// TODO: Check for overflows.
				goto do_over;
			}
			currWindow = WindowPtr(WindowPeek(currWindow)->nextWindow);
		}
		foundUnique = true;
	}
	
	if (foundUnique) {
		SetTitle(candidateTitle);
	}
}

// Also called for some click events that bring the window to front.
void CWindow::HandleUpdate(const EventRecord &event) {
	GrafPtr oldPort = NULL;
	StPortChanger portSaver(mWindow);
	BeginUpdate(mWindow);
	
	DrawContents();
		
	CWindowAttachment *currAttachment = mAttachments;
	while (currAttachment) {
		currAttachment->Draw();
		
		currAttachment = currAttachment->mNext;
	}
	
	Rect growArea = mWindow->portRect;
	growArea.left = growArea.right - 15;
	growArea.top = growArea.bottom - 15;
	{
		StClipSaver clipSaver;
		ClipRect(&growArea);
		DrawGrowIcon(mWindow);
	}
	EndUpdate(mWindow);
}


void CWindow::HandleDrag(const EventRecord &event) {
	if ((event.modifiers & cmdKey) == 0) {
		CWindow * fw = Frontmost();
		if (fw && fw != this && fw->IsModal()) {
			SysBeep(007);
			return;
		}
		
		SelectWindow(mWindow);
		HandleUpdate(event); // Force redraw of newly-exposed areas.
	}
	DragWindow(mWindow, event.where, &(**GetGrayRgn()).rgnBBox);
}


void CWindow::HandleResize(const EventRecord &event) {
	CWindowAttachment *currAttachment = NULL;
	Rect limits;
	if ((event.modifiers & cmdKey) == 0) {
		CWindow * fw = Frontmost();
		if (fw && fw != this && fw->IsModal()) {
			SysBeep(007);
			return;
		}
		
		SelectWindow(mWindow);
	}
	GetResizeLimits(&limits);
	
	Point oldSize = *(Point*)&mWindow->portRect.bottom;
	
	long desiredSize = 0;
	desiredSize = GrowWindow(mWindow, event.where, &limits);
	if (desiredSize != 0) {
		SizeWindow(mWindow, LoWord(desiredSize), HiWord(desiredSize), true);
		
		CControl* currControlObject = mControls;
		while (currControlObject != NULL) {
			currControlObject->AutoPositionInWindow();
			currControlObject = currControlObject->mNext;
		}

		currAttachment = mAttachments;
		while (currAttachment) {
			currAttachment->ResizedWindowFrom(oldSize);
			
			currAttachment = currAttachment->mNext;
		}
	}
}


void CWindow::HandleClose(const EventRecord &event) {
	if (TrackGoAway(mWindow, event.where) && ShouldClose()) {
		delete this;
	}
}


void CWindow::HandleZoom(const EventRecord &event, short part) {
	if (TrackBox(mWindow, event.where, part)) {
		Rect wBox = mWindow->portRect;
		LocalToGlobalRect(&wBox);
		//if ((**WStateDataHandle(WindowPeek(mWindow)->dataHandle)).stdState
	}
}

void CWindow::HandleCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'clos') {
		if (ShouldClose()) {
			delete this;
		}
	} else {
		mNextHandler->HandleCommand(command, menuID, itemIndex);
	}
}

void CWindow::UpdateCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'clos') {
		EnableItem(GetMenuHandle(menuID), itemIndex);
	} else {
		CCommandHandler::UpdateCommand(command, menuID, itemIndex);
	}
}


void CWindow::Activate() {
	HiliteWindow(mWindow, true);
	
	CControl* currControlObject = mControls;
	while (currControlObject != NULL) {
		currControlObject->Activate();
		currControlObject = currControlObject->mNext;
	}
	
	CWindowAttachment *currAttachment = mAttachments;
	while (currAttachment) {
		currAttachment->Activate();
		
		currAttachment = currAttachment->mNext;
	}
}


void CWindow::Deactivate() {
	HiliteWindow(mWindow, false);
	
	CControl* currControlObject = mControls;
	while (currControlObject != NULL) {
		currControlObject->Deactivate();
		currControlObject = currControlObject->mNext;
	}
	
	CWindowAttachment *currAttachment = mAttachments;
	while (currAttachment) {
		currAttachment->Deactivate();
		
		currAttachment = currAttachment->mNext;
	}
}

void CWindow::AddAttachment(CWindowAttachment * att) {
	att->mNext = mAttachments;
	mAttachments = att;
}

void CWindow::AddControl(CControl *ctl) {
	ctl->mNext = mControls;
	mControls = ctl;
}


void CWindow::LocalToGlobalPoint(Point *pos) {
	StPortChanger portSaver(mWindow);
	LocalToGlobal(pos);
}

void CWindow::LocalToGlobalRect(Rect *box) {
	StPortChanger portSaver(mWindow);
	Point pos = { 0, 0 };
	LocalToGlobal(&pos);
	OffsetRect(box, pos.h, pos.v);
}

void CWindow::GlobalToLocalPoint(Point *pos) {
	StPortChanger portSaver(mWindow);
	GlobalToLocal(pos);
}

void CWindow::GlobalToLocalRect(Rect *box) {
	StPortChanger portSaver(mWindow);
	Point pos = { 0, 0 };
	GlobalToLocal(&pos);
	OffsetRect(box, pos.h, pos.v);
}

CWindow* CWindow::Frontmost() {
	WindowPtr fw = FrontWindow();
	if (!fw || fw == WindowPtr(SIOUXTextWindow)) {
		return NULL;
	}
	return (CWindow*)GetWRefCon(fw);
}
