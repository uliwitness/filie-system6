#pragma once

#include "CWindow.h"

class CListManagerWindowAttachment;

class CFilieWindow : public CWindow {
public:
	CFilieWindow(Rect *box, FSSpec *fileOrFolder = NULL, CCommandHandler* parent = NULL);

	virtual void CreateWindow();
	
	virtual void HandleCommand(OSType command, short menuID, short itemIndex);
	
protected:
	CListManagerWindowAttachment *mList;
	Boolean mListVolumes;
	FSSpec mFileOrFolder;
};