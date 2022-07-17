#pragma once

#include <Types.h>

// Fake FSSpec because System 6 doesn't have FSSpecs yet.
class CFileSpec {
public:
	short vRefNum;
	long parID;
	Str255 name;
	
	CFileSpec(short vrn = 0, short pid = 0, Str255 nm = "\p");
	CFileSpec(FSSpec* fssp);
	
protected:
	void	Normalize();
};
