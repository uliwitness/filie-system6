#include "CButtonControl.h"#include "CWindow.h"CButtonControl::CButtonControl(CWindow* wd, Rect* box, Str255 title, OSType command): CControl(wd, box, title, 0, 0, 0, pushButProc), mCommand(command) {}void CButtonControl::HandleClick(ControlPartCode hitPart) {	if (hitPart == kControlButtonPart) {		CWindow * owner = CWindow::FromMac((**mControlHandle).contrlOwner);		owner->HandleCommand(mCommand, 0, 0);	}}