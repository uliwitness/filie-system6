#include "CDialog.h"
#include "CControl.h"
#include "SMenuCommands.h"

static Rect gDummyBox = {};

CDialog::CDialog(short dlogResourceID, CCommandHandler* parent)
	: CWindow(&gDummyBox, parent), mResID(dlogResourceID) {
}

CDialog::~CDialog() {
	FreeWindow();
}


void CDialog::CreateWindow() {
	mWindow = GetNewDialog(mResID, NULL, (WindowPtr) -1);
	SetWRefCon(mWindow, (long)this);
}

void CDialog::FreeWindow() {
	if (mWindow) { // In case a subclass already cleaned up.
		DisposeDialog(mWindow);
		mWindow = NULL;
	}
}

void CDialog::DrawContents() {
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
	
	UpdateDialog(mWindow, mWindow->visRgn);
}

void CDialog::HandleItemClick(short itemIndex) {
	SMenuCommandsHandle commands = (SMenuCommandsHandle) GetResource('DITC', mResID);
	if (itemIndex <= (**commands).count) {
		OSType command = (**commands).commands[itemIndex - 1];
		if (command != 0 && command != '----') {
			HandleCommand(command, 0, 0);
		}
	}
}

void CDialog::UpdateCommand(OSType command, short menuID, short itemIndex) {
	if (command == 0 && menuID == 128) {
		EnableItem(GetMenuHandle(menuID), itemIndex);
		return;
	}
	
	DisableItem(GetMenuHandle(menuID), itemIndex);
}

