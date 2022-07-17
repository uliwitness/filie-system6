#include "CFilieWindow.h"
#include "CListManagerWindowAttachment.h"
#include "CApplication.h"
#include "FileLDEFData.h"
#include <string.h>
#include <limits.h>
#include <stdio.h>


enum {
	kFolderBit = (1 << 4)
};

CFilieWindow::CFilieWindow(CFileSpec *fileOrFolder, CCommandHandler* parent)
: CWindow(128, parent) {
	if (fileOrFolder) {
		mFileOrFolder = *fileOrFolder;
		mListVolumes = false;
	} else {
		mListVolumes = true;
	}
}


static Handle sDiskIcon = NULL;
static Handle sFolderIcon = NULL;
static Handle sFileIcon = NULL;
static Handle sApplicationIcon = NULL;


void CFilieWindow::CreateWindow() {
	CWindow::CreateWindow();
	
	if (!sDiskIcon) {
		sDiskIcon = GetResource('ICN#', floppyIconResource);
		DetachResource(sDiskIcon);
		HNoPurge(sDiskIcon);
		sFolderIcon = GetResource('ICN#', genericFolderIconResource);
		DetachResource(sFolderIcon);
		HNoPurge(sFolderIcon);
		sFileIcon = GetResource('ICN#', genericDocumentIconResource);
		DetachResource(sFileIcon);
		HNoPurge(sFileIcon);
		sApplicationIcon = GetResource('ICN#', genericApplicationIconResource);
		DetachResource(sApplicationIcon);
		HNoPurge(sApplicationIcon);
		printf("icons %p %p %p %p\n", sDiskIcon, sFolderIcon, sFileIcon, sApplicationIcon);
	}

	struct FileEntry fileEntry = {};
	Rect listRect = mWindow->portRect;
	listRect.top += 32;
	listRect.bottom -= 15;
	mList = new CListManagerWindowAttachment(this, &listRect, 700);
	mList->SetAutoPositionFlags(kAutoPositionFlagResizeHorz | kAutoPositionFlagResizeVert);
	mList->SetCommand('ldbl');
	
	if (mListVolumes) {
		HParamBlockRec paramBlock = {};
		
		for (paramBlock.volumeParam.ioVolIndex = 1; paramBlock.volumeParam.ioVolIndex <= SHRT_MAX; ++paramBlock.volumeParam.ioVolIndex) {
			paramBlock.volumeParam.ioNamePtr = fileEntry.file.name;
			OSErr err = PBHGetVInfo(&paramBlock, false);
			if (err != noErr) {
				break;
			}
			
			fileEntry.icon = sDiskIcon;
			fileEntry.file.vRefNum = paramBlock.volumeParam.ioVRefNum;
			fileEntry.file.parID = fsRtParID;
			mList->AddRow(&fileEntry, sizeof(fileEntry));
		}
	} else {
		CInfoPBRec	catInfo = {0};
		
		BlockMove(mFileOrFolder.name, fileEntry.file.name, mFileOrFolder.name[0] + 1);	
		catInfo.dirInfo.ioVRefNum = mFileOrFolder.vRefNum;
		catInfo.dirInfo.ioDrDirID = mFileOrFolder.parID;
		catInfo.dirInfo.ioFDirIndex = 0;
		catInfo.dirInfo.ioNamePtr = fileEntry.file.name;
		
		OSErr err = PBGetCatInfoSync(&catInfo);
		if (err != noErr) {
			return;
		}
		
		short vRefNum = catInfo.dirInfo.ioVRefNum;
		long dirID = catInfo.dirInfo.ioDrDirID;
	
		for (int dirIndex = 1; dirIndex < SHRT_MAX; ++dirIndex) {
			
			fileEntry.file.name[0] = 0;
			catInfo.dirInfo.ioVRefNum = vRefNum;
			catInfo.dirInfo.ioDrDirID = dirID;
			catInfo.dirInfo.ioFDirIndex = dirIndex;
			catInfo.dirInfo.ioNamePtr = fileEntry.file.name;

			err = PBGetCatInfoSync(&catInfo);
			if (err == fnfErr) {
				break;
			} else if (err != noErr) {
				return; // TODO: Report error somehow.
			}
			
			
			if ((catInfo.hFileInfo.ioFlFndrInfo.fdFlags & fInvisible) == fInvisible) { // Skip invisible files.
				continue;
			}

			Boolean isFolder = (catInfo.hFileInfo.ioFlAttrib & kFolderBit) == kFolderBit;
			
			if (isFolder) {
				fileEntry.icon = sFolderIcon;
				fileEntry.file.vRefNum = catInfo.dirInfo.ioVRefNum;
				fileEntry.file.parID = catInfo.dirInfo.ioDrParID;
				mList->AddRow(&fileEntry, sizeof(fileEntry));
			} else {
				if (catInfo.hFileInfo.ioFlFndrInfo.fdType == 'APPL') {
					fileEntry.icon = sApplicationIcon;
				} else {
					fileEntry.icon = sFileIcon;
				}
				fileEntry.file.vRefNum = catInfo.hFileInfo.ioVRefNum;
				fileEntry.file.parID = catInfo.hFileInfo.ioDirID;
				mList->AddRow(&fileEntry, sizeof(fileEntry));
			}
			
		}
	}
}

void CFilieWindow::HandleCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'ldbl') {
		CFileSpec spec;
		short len = sizeof(spec);
		mList->GetSelectedRow(&spec, &len);
		CApplication::Singleton()->GetDelegate()->OpenDocument(&spec);
	} else {
		CWindow::HandleCommand(command, menuID, itemIndex);
	}
}
