#include "CScrollbarControl.h"
#include "CWindow.h"
#include "StPortChanger.h"
#include <stdio.h>

CScrollbarControl::CScrollbarControl(CWindow* wd, Rect* box, short value, short min, short max, OSType command)
: CControl(wd, box, "\p", value, min, max, scrollBarProc), mCommand(command) {
}

void CScrollbarControl::HandleClick(ControlPartCode hitPart) {
	CWindow * owner = CWindow::FromMac((**mControlHandle).contrlOwner);
	switch (hitPart) {
		case kControlUpButtonPart:
		case kControlDownButtonPart:
		case kControlPageUpPart:
		case kControlPageDownPart:
		case kControlIndicatorPart:
			owner->HandleCommand(mCommand, 0, 0);
			printf("handleClick %ld\n", mCommand);
			break;
	}
}

void CScrollbarControl::HandleMouseDown(const EventRecord &event) {
	StPortChanger portSaver((**mControlHandle).contrlOwner);
	Point pos = event.where;
	GlobalToLocal(&pos);
	ControlPartCode part = TrackControl(mControlHandle, pos, ScrollbarAction);
	if (part != kControlNoPart) {
		HandleClick(part);
	}
}

pascal void CScrollbarControl::ScrollbarAction(ControlRef theControl, ControlPartCode partCode) {
	switch (partCode) {
		case kControlUpButtonPart:
			SetControlValue(theControl, GetControlValue(theControl) - 1);
			break;
			
		case kControlDownButtonPart:
			SetControlValue(theControl, GetControlValue(theControl) + 1);
			break;
			
		case kControlPageUpPart:
			SetControlValue(theControl, GetControlValue(theControl) - 10);
			break;
			
		case kControlPageDownPart:
			SetControlValue(theControl, GetControlValue(theControl) + 10);
			break;
			
		case kControlIndicatorPart:
			break;
	}
}