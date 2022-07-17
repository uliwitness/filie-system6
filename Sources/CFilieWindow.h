#pragma once

#include "CWindow.h"
#include "CFileSpec.h"

class CListManagerWindowAttachment;

class CFilieWindow : public CWindow {
public:
	CFilieWindow(CFileSpec *fileOrFolder = NULL, CCommandHandler* parent = NULL);

	virtual void CreateWindow();
	
	virtual void HandleCommand(OSType command, short menuID, short itemIndex);
	
protected:
	CListManagerWindowAttachment *mList;
	Boolean mListVolumes;
	CFileSpec mFileOrFolder;
};