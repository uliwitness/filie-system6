#include "CControl.h"
#include "CWindow.h"
#include "StPortChanger.h"

CControl::CControl(CWindow* wd, Rect* box, Str255 title, SInt16 value, SInt16 min, SInt16 max, SInt16 procID)
: mAutoPositionFlags(kAutoPositionFlagsPinTopLeft), mNext(NULL) {
	WindowPtr window = wd->ToMac();
	mControlHandle = NewControl(window, box, title, true, value, min, max, procID, (long)this);
	mLeftDistance = box->left;
	mTopDistance = box->top;
	mRightDistance = window->portRect.right - box->right;
	mBottomDistance = window->portRect.bottom - box->bottom;
	wd->AddControl(this);
}

void CControl::AutoPositionInWindow() {
	Rect controlBox = (**mControlHandle).contrlRect;
	if ((mAutoPositionFlags & kAutoPositionFlagsPinTopLeft) == kAutoPositionFlagsPinTopLeft) {
		return;
	}
	if ((mAutoPositionFlags & kAutoPositionFlagResizeHorz) == kAutoPositionFlagResizeHorz) {
		controlBox.left = mLeftDistance;
		controlBox.right = (**mControlHandle).contrlOwner->portRect.right - mRightDistance;
	} else if ((mAutoPositionFlags & kAutoPositionFlagPinRight) == kAutoPositionFlagPinRight) {
		short width = controlBox.right - controlBox.left;
		controlBox.right = (**mControlHandle).contrlOwner->portRect.right - mRightDistance;
		controlBox.left = controlBox.right - width;
	}

	if ((mAutoPositionFlags & kAutoPositionFlagResizeVert) == kAutoPositionFlagResizeVert) {
		controlBox.top = mTopDistance;
		controlBox.bottom = (**mControlHandle).contrlOwner->portRect.bottom - mBottomDistance;
	} else if ((mAutoPositionFlags & kAutoPositionFlagPinBottom) == kAutoPositionFlagPinBottom) {
		short height = controlBox.bottom - controlBox.top;
		controlBox.bottom = (**mControlHandle).contrlOwner->portRect.bottom - mBottomDistance;
		controlBox.top = controlBox.bottom - height;
	}
	
	MoveControl(mControlHandle, controlBox.left, controlBox.top);
	SizeControl(mControlHandle, controlBox.right - controlBox.left, controlBox.bottom - controlBox.top);
}

void CControl::HandleMouseDown(const EventRecord &event) {
	StPortChanger portSaver((**mControlHandle).contrlOwner);
	Point pos = event.where;
	GlobalToLocal(&pos);
	ControlPartCode part = TrackControl(mControlHandle, pos, (ControlActionProcPtr) -1);
	if (part != kControlNoPart) {
		HandleClick(part);
	}
}

void CControl::Activate() {
	HiliteControl(mControlHandle, kControlNoPart);
}

void CControl::Deactivate() {
	HiliteControl(mControlHandle, kControlInactivePart);
}
