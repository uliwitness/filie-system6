#pragma once

#include <QuickDraw.h>

class StClipSaver {
public:
	StClipSaver() { mClipRegion = NewRgn(); GetClip(mClipRegion); }
	~StClipSaver() { SetClip(mClipRegion); DisposeRgn(mClipRegion); mClipRegion = NULL; }
	
protected:
	RgnHandle mClipRegion;
};