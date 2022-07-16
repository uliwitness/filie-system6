#include "CWindowAttachment.h"
#include "CWindow.h"

CWindowAttachment::CWindowAttachment(CWindow *wd)
: CCommandHandler(wd), mWindow(wd), mNext(NULL) {
	wd->AddAttachment(this);
}

void CWindowAttachment::ResizedFromOldToNewWindowRect(Rect *box, Point oldSize, Rect *newWdRect) {
	short leftDistance, topDistance, rightDistance, bottomDistance;
	
	leftDistance = box->left;
	topDistance = box->top;
	rightDistance = oldSize.h - box->right;
	bottomDistance = oldSize.v - box->bottom;
	
	if (mAutoPositionFlags == kAutoPositionFlagsPinTopLeft) {
		return;
	}
	if ((mAutoPositionFlags & kAutoPositionFlagResizeHorz) == kAutoPositionFlagResizeHorz) {
		box->left = leftDistance;
		box->right = newWdRect->right - rightDistance;
	} else if ((mAutoPositionFlags & kAutoPositionFlagPinRight) == kAutoPositionFlagPinRight) {
		short width = box->right - box->left;
		box->right = newWdRect->right - rightDistance;
		box->left = box->right - width;
	}

	if ((mAutoPositionFlags & kAutoPositionFlagResizeVert) == kAutoPositionFlagResizeVert) {
		box->top = topDistance;
		box->bottom = newWdRect->bottom - bottomDistance;
	} else if ((mAutoPositionFlags & kAutoPositionFlagPinBottom) == kAutoPositionFlagPinBottom) {
		short height = box->bottom - box->top;
		box->bottom = newWdRect->bottom - bottomDistance;
		box->top = box->bottom - height;
	}
}
