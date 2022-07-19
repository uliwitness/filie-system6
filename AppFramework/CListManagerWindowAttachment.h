#pragma once

#include "CWindowAttachment.h"
#include <Lists.h>

class CListManagerWindowAttachment : public CWindowAttachment {
public:
	CListManagerWindowAttachment(CWindow *wd, Rect *box, short ldefID = 0);
	~CListManagerWindowAttachment();
	
	void AddRow(const void* data, short length);
	void GetSelectedRow(void* outData, short *ioLength);
	
	void SetCommand(OSType command) { mCommand = command; }
	
	void SetAutoDraw(Boolean state);
	
	virtual void Draw();
	
	virtual bool HandleMouseDown(const EventRecord &event);
	
	virtual void ResizedWindowFrom(Point oldSize);
	
	virtual void Activate();
	virtual void Deactivate();
	
protected:
	void InvalidateListRect();

	ListHandle mList;
	OSType mCommand;
};
