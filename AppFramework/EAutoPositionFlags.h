#pragma once

enum {
	kAutoPositionFlagPinLeft = (1 << 0),
	kAutoPositionFlagPinRight = (1 << 1),
	kAutoPositionFlagResizeHorz = (kAutoPositionFlagPinLeft | kAutoPositionFlagPinRight),
	kAutoPositionFlagPinTop = (1 << 2),
	kAutoPositionFlagPinBottom = (1 << 3),
	kAutoPositionFlagResizeVert = (kAutoPositionFlagPinTop | kAutoPositionFlagPinBottom),
	kAutoPositionFlagsPinTopLeft = (kAutoPositionFlagPinLeft | kAutoPositionFlagPinTop)
};
typedef UInt8 EAutoPositionFlags;
