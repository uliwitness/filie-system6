#pragma once

#include <Types.h>

class CStream {
public:
	CStream(Ptr pos, long bytesLeft);
	
	short ReadShort();
	long ReadLong();
	OSType ReadOSType();
	
	long GetBytesLeft() { return mBytesLeft; }
	
protected:
	long mBytesLeft;
	Ptr mPos;
};