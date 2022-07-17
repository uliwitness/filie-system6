#pragma once

#include "CCommandHandler.h"
#include <Windows.h>


class CWindowAttachment;
class CControl;


// Class for one window:
class CWindow : public CCommandHandler {
public:
	CWindow(short resID, CCommandHandler *parent = NULL);
	virtual ~CWindow();

	virtual void CreateWindow();

	virtual Boolean ShouldClose() { return true; }

	virtual void DrawContents();

	virtual void GetResizeLimits(Rect *outBox) { SetRect(outBox, 64, 64, 32767, 32767); }

	virtual void HandleCommand(OSType command, short menuID, short itemIndex);
	virtual void UpdateCommand(OSType command, short menuID, short itemIndex);
	
	void SetUniqueTitle(ConstStr255Param title);
	void SetTitle(ConstStr255Param title) { SetWTitle(mWindow, title); }
	
	virtual void Activate();
	virtual void Deactivate();
	
	virtual Boolean IsModal() { return false; }
	
	virtual void HandleMouseDown(const EventRecord &event);
	virtual void HandleMouseUp(const EventRecord &event);
	virtual void HandleDrag(const EventRecord &event);
	virtual void HandleResize(const EventRecord &event);
	virtual void HandleClose(const EventRecord &event);
	virtual void HandleZoom(const EventRecord &event, short part);
	virtual void HandleUpdate(const EventRecord &event);

	void LocalToGlobalPoint(Point *pos);
	void LocalToGlobalRect(Rect *box);

	void GlobalToLocalPoint(Point *pos);
	void GlobalToLocalRect(Rect *box);

	WindowPtr ToMac() { return mWindow; }
	
	static CWindow* FromMac(WindowPtr macWindow) { return (CWindow*)GetWRefCon(macWindow); }
	static CWindow* Frontmost();
	
protected:
	virtual void FreeWindow();
	void AddAttachment(CWindowAttachment * att);
	void AddControl(CControl *ctl);

	short mResID;
	WindowPtr mWindow;
	CWindowAttachment *mAttachments; // Linked list of window attachments (e.g. list boxes, Text boxes etc.)
	CControl *mControls; // Linked list of control objects.
	
	friend class CWindowAttachment;
	friend class CControl;
};