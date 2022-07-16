#pragma once

#include "CControl.h"

class CButtonControl : public CControl {
public:
	CButtonControl(CWindow* wd, Rect* box, Str255 title, OSType command);

	virtual void HandleClick(ControlPartCode hitPart);
	
	void SetCommand(OSType cmd) { mCommand = cmd; }
	
protected:
	OSType mCommand;
};