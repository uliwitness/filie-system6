#pragma once

#include <QuickDraw.h>

class StPortChanger {
public:
	StPortChanger(GrafPtr newPort = NULL) { GetPort(&mOldPort); if (newPort) { SetPort(newPort); } }
	~StPortChanger() { SetPort(mOldPort); mOldPort = NULL; }
	
protected:
	GrafPtr mOldPort;
};