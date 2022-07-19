#include "CFilieWindow.h"
#include "CListManagerWindowAttachment.h"
#include "CApplication.h"
#include "FileLDEFData.h"
#include "CStream.h"
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

struct DesktopDBEntry {
	short unknown1;
	short unknown2;
	short bndlResID;
};

struct FileReferenceResource {
	OSType fileType;
	short localID;
	Str255 fileName;
};

struct BundleResourceIconEntry {
	short localID;
	short resID;
};

struct BundleResourceTypeEntry {
	OSType type; // FREF, ICN# etc.
	short numIcons; // -1
	BundleResourceIconEntry icons[];
};

struct BundleResource {
	OSType creator;
	short ownerID;
	short numTypes; // -1
	struct BundleResourceTypeEntry types[];
};

static Handle FindIconForType(OSType fdType, OSType fdCreator) {
	Handle icon = NULL;
	struct DesktopDBEntry **desktopDBEntry = (struct DesktopDBEntry **) GetResource(fdCreator, 0);
	if (!desktopDBEntry || GetHandleSize((Handle)desktopDBEntry) != sizeof(struct DesktopDBEntry)) {
		return NULL;
	}
	
	short bndlID = (**desktopDBEntry).bndlResID;
	Handle bundleRes = GetResource('BNDL', bndlID);
	HLock(bundleRes);
	
	short foundLocalID = SHRT_MIN;

	{
		CStream stream(*bundleRes, GetHandleSize(bundleRes));
		OSType creator = stream.ReadOSType();
		
		short ownerID = stream.ReadShort();
		short numTypes = stream.ReadShort();
		for (short typeIndex = 0; typeIndex <= numTypes; ++typeIndex) {
			OSType type = stream.ReadOSType();
			short numIcons = stream.ReadShort();
			for (short iconIndex = 0; iconIndex <= numIcons; ++iconIndex) {
				short localID = stream.ReadShort();
				short resID = stream.ReadShort();
				
				if (type == 'FREF' && fdCreator == creator) {
					struct FileReferenceResource **fref = (struct FileReferenceResource **)GetResource('FREF', resID);
					if ((**fref).fileType == fdType) {
						foundLocalID = (**fref).localID;
					}
				}
			}
		}
	}
	
	if (foundLocalID != SHRT_MIN) {
		CStream stream(*bundleRes, GetHandleSize(bundleRes));
		OSType creator = stream.ReadOSType();
		
		short ownerID = stream.ReadShort();
		short numTypes = stream.ReadShort();
		for (short typeIndex = 0; typeIndex <= numTypes; ++typeIndex) {
			OSType type = stream.ReadOSType();
			short numIcons = stream.ReadShort();
			for (short iconIndex = 0; iconIndex <= numIcons; ++iconIndex) {
				short localID = stream.ReadShort();
				short resID = stream.ReadShort();
				
				if (type == 'ICN#' && fdCreator == creator && localID == foundLocalID) {
					icon = GetResource('ICN#', resID);
				}
			}
		}
	}

	HUnlock(bundleRes);
	
	return icon;
}


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
	}

	struct FileEntry fileEntry = {};
	Str255 nameBuffer = {};
	Rect listRect = mWindow->portRect;
	listRect.top += 32;
	listRect.bottom -= 15;
	mList = new CListManagerWindowAttachment(this, &listRect, 700);
	mList->SetAutoPositionFlags(kAutoPositionFlagResizeHorz | kAutoPositionFlagResizeVert);
	mList->SetCommand('ldbl');
	mList->SetAutoDraw(false);
	
	if (mListVolumes) {
		HParamBlockRec paramBlock = {};
		
		for (paramBlock.volumeParam.ioVolIndex = 1; paramBlock.volumeParam.ioVolIndex <= SHRT_MAX; ++paramBlock.volumeParam.ioVolIndex) {
			paramBlock.volumeParam.ioNamePtr = nameBuffer;
			OSErr err = PBHGetVInfo(&paramBlock, false);
			if (err != noErr) {
				break;
			}
			
			ParamBlockRec deviceParamBlock = {};
			deviceParamBlock.cntrlParam.ioCRefNum = paramBlock.volumeParam.ioVDRefNum;
			deviceParamBlock.cntrlParam.ioVRefNum = paramBlock.volumeParam.ioVDrvInfo;
			deviceParamBlock.cntrlParam.csCode = 22;
			err = PBControlSync(&deviceParamBlock);
			if (err != noErr) {
				deviceParamBlock.cntrlParam.csCode = 21;
				err = PBControlSync(&deviceParamBlock);
			}
			
			Handle icon = sDiskIcon;
			Ptr iconPtr = *(Ptr*)&deviceParamBlock.cntrlParam.csParam;
			if (PtrToHand(iconPtr, &icon, 128) != noErr) { // FIXME! icon Handle is leaked.
				icon = sDiskIcon;
			}
			
			// Generate spec for Desktop file and open it.
			
			fileEntry.icon = icon;
			fileEntry.file.vRefNum = paramBlock.volumeParam.ioVRefNum;
			fileEntry.file.parID = fsRtParID;
			BlockMove(nameBuffer, fileEntry.file.name, sizeof(fileEntry.file.name));
			mList->AddRow(&fileEntry, sizeof(fileEntry));
		}
	} else {
		CInfoPBRec	catInfo = {0};
		
		BlockMove(mFileOrFolder.name, nameBuffer, mFileOrFolder.name[0] + 1);	
		catInfo.dirInfo.ioVRefNum = mFileOrFolder.vRefNum;
		catInfo.dirInfo.ioDrDirID = mFileOrFolder.parID;
		catInfo.dirInfo.ioFDirIndex = 0;
		catInfo.dirInfo.ioNamePtr = nameBuffer;
		
		OSErr err = PBGetCatInfoSync(&catInfo);
		if (err != noErr) {
			return;
		}
		
		short vRefNum = catInfo.dirInfo.ioVRefNum;
		long dirID = catInfo.dirInfo.ioDrDirID;
	
		for (int dirIndex = 1; dirIndex < SHRT_MAX; ++dirIndex) {
			
			nameBuffer[0] = 0;
			catInfo.dirInfo.ioVRefNum = vRefNum;
			catInfo.dirInfo.ioDrDirID = dirID;
			catInfo.dirInfo.ioFDirIndex = dirIndex;
			catInfo.dirInfo.ioNamePtr = nameBuffer;

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
			
			BlockMove(nameBuffer, fileEntry.file.name, sizeof(fileEntry.file.name));
			if (isFolder) {
				fileEntry.icon = sFolderIcon;
				fileEntry.file.vRefNum = catInfo.dirInfo.ioVRefNum;
				fileEntry.file.parID = catInfo.dirInfo.ioDrParID;
				mList->AddRow(&fileEntry, sizeof(fileEntry));
			} else {
				if (fileEntry.icon = FindIconForType(catInfo.hFileInfo.ioFlFndrInfo.fdType, catInfo.hFileInfo.ioFlFndrInfo.fdCreator)) {
					// Success! Real icon!
				} else if (catInfo.hFileInfo.ioFlFndrInfo.fdType == 'APPL') {
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
	mList->SetAutoDraw(true);
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
