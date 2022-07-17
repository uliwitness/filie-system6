#pragma once

#include "CApplication.h"

class CFilieApplicationDelegate : public CApplicationDelegate {
public:
	CFilieApplicationDelegate(CCommandHandler* parent);
	
	virtual void OpenUntitledDocument();
	virtual void OpenDocument(CFileSpec *fileOrFolder);

	virtual void HandleCommand(OSType command, short menuID, short itemIndex);
	virtual void UpdateCommand(OSType command, short menuID, short itemIndex);

};