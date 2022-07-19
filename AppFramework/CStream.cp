#include "CStream.h"

CStream::CStream(Ptr pos, long bytesLeft)
: mPos(pos), mBytesLeft(bytesLeft) {
}

short CStream::ReadShort() {
	short result = 0;
	if (mBytesLeft < sizeof(result)) {
		return 0;
	}
	BlockMove(mPos, &result, sizeof(result));
	mPos += sizeof(result);
	mBytesLeft -= sizeof(result);
	return result;
}

long CStream::ReadLong() {
	long result = 0;
	if (mBytesLeft < sizeof(result)) {
		return 0;
	}
	BlockMove(mPos, &result, sizeof(result));
	mPos += sizeof(result);
	mBytesLeft -= sizeof(result);
	return result;
}

OSType CStream::ReadOSType() {
	OSType result = 0;
	if (mBytesLeft < sizeof(result)) {
		return 0;
	}
	BlockMove(mPos, &result, sizeof(result));
	mPos += sizeof(result);
	mBytesLeft -= sizeof(result);
	return result;
}

