#include "CFileSpec.h"

CFileSpec::CFileSpec(short vrn, short pid, Str255 nm)
: vRefNum(vrn), parID(pid) {
	BlockMove(nm, name, nm[0] + 1);
	if (name[0] < 255) {
		name[name[0] + 1] = 0; // Zero-terminate for easier debugging.
	}
}

CFileSpec::CFileSpec(FSSpec* fssp)
: vRefNum(fssp->vRefNum), parID(fssp->parID) {
	BlockMove(fssp->name, name, fssp->name[0] + 1);
}

void	CFileSpec::Normalize() {
	if (name[0] == 0) {
		CInfoPBRec	catInfo = {0};
		Str255		fileNameBuffer = {0};
			
		catInfo.dirInfo.ioVRefNum = vRefNum;
		catInfo.dirInfo.ioDrDirID = parID;
		catInfo.dirInfo.ioFDirIndex = -1;
		catInfo.dirInfo.ioNamePtr = name;
		
		OSErr err = PBGetCatInfoSync(&catInfo);
		if (err != noErr) {
			return;
		}
		
		vRefNum = catInfo.dirInfo.ioVRefNum;
		parID = catInfo.dirInfo.ioDrParID;
		if (name[0] < 255) {
			name[name[0] + 1] = 0; // Zero-terminate for easier debugging.
		}
	}
}
