#include "CFilieWindow.h"
#include "CListManagerWindowAttachment.h"
#include "CApplication.h"
#include <string.h>
#include <limits.h>


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


void CFilieWindow::CreateWindow() {
	CWindow::CreateWindow();

	Rect listRect = mWindow->portRect;
	mList = new CListManagerWindowAttachment(this, &listRect, true, 700);
	mList->SetAutoPositionFlags(kAutoPositionFlagResizeHorz | kAutoPositionFlagResizeVert);
	mList->SetCommand('ldbl');
	
	if (mListVolumes) {
		HParamBlockRec paramBlock = {};
		Str255 volName = {};
		paramBlock.volumeParam.ioNamePtr = volName;
		
		for (paramBlock.volumeParam.ioVolIndex = 1; paramBlock.volumeParam.ioVolIndex <= SHRT_MAX; ++paramBlock.volumeParam.ioVolIndex) {
			OSErr err = PBHGetVInfo(&paramBlock, false);
			if (err != noErr) {
				break;
			}
			
			CFileSpec spec(paramBlock.volumeParam.ioVRefNum, fsRtParID, volName);
			mList->AddRow(&spec, sizeof(spec));
		}
	} else {
		CInfoPBRec	catInfo = {0};
		Str255		fileNameBuffer = {0};
		
		BlockMove(mFileOrFolder.name, fileNameBuffer, mFileOrFolder.name[0] + 1);	
		catInfo.dirInfo.ioVRefNum = mFileOrFolder.vRefNum;
		catInfo.dirInfo.ioDrDirID = mFileOrFolder.parID;
		catInfo.dirInfo.ioFDirIndex = 0;
		catInfo.dirInfo.ioNamePtr = fileNameBuffer;
		
		OSErr err = PBGetCatInfoSync(&catInfo);
		if (err != noErr) {
			return;
		}
		
		short vRefNum = catInfo.dirInfo.ioVRefNum;
		long dirID = catInfo.dirInfo.ioDrDirID;
	
		for (int dirIndex = 1; dirIndex < SHRT_MAX; ++dirIndex) {
			
			fileNameBuffer[0] = 0;
			catInfo.dirInfo.ioVRefNum = vRefNum;
			catInfo.dirInfo.ioDrDirID = dirID;
			catInfo.dirInfo.ioFDirIndex = dirIndex;
			catInfo.dirInfo.ioNamePtr = fileNameBuffer;

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
				CFileSpec fldSpec(catInfo.dirInfo.ioVRefNum, catInfo.dirInfo.ioDrParID, catInfo.dirInfo.ioNamePtr);
				mList->AddRow(&fldSpec, sizeof(fldSpec));
			} else {
				CFileSpec filSpec(catInfo.hFileInfo.ioVRefNum, catInfo.hFileInfo.ioDirID, catInfo.hFileInfo.ioNamePtr);
				mList->AddRow(&filSpec, sizeof(filSpec));
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
