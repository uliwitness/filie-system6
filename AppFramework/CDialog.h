#pragma once

#include "CWindow.h"

class CDialog : public CWindow {
public:
	CDialog(short dlogResourceID, CCommandHandler* parent = NULL);
	~CDialog();
	
	virtual void HandleItemClick(short itemIndex);

	virtual void CreateWindow();
	
	virtual void DrawContents();
	
	virtual void Activate() {}
	virtual void Deactivate() {}
	
	virtual Boolean IsModal() { return true; }
	
	virtual void UpdateCommand(OSType command, short menuID, short itemIndex);
	
protected:
	virtual void FreeWindow();
};