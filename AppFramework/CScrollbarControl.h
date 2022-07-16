#pragma once

#include "CControl.h"

class CScrollbarControl : public CControl {
public:
	CScrollbarControl(CWindow* wd, Rect* box, short value, short min, short max, OSType command);

	void SetCommand(OSType cmd) { mCommand = cmd; }
	
	virtual void HandleClick(ControlPartCode hitPart);
	virtual void HandleMouseDown(const EventRecord& event);

protected:
	static pascal void ScrollbarAction(ControlRef theControl, ControlPartCode partCode);
	
	OSType mCommand;
};