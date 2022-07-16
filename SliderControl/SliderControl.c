#include <Controls.h>
#include <QuickDraw.h>
#include <SetUpA4.h>
#include <A4Stuff.h>
#include "SliderControl.h"

void CalculateTrackRect(short varCode, ControlHandle theControl, Rect *outRect) {
	short heightToRemove = 0;
	*outRect = (**theControl).contrlRect;
	heightToRemove = (outRect->bottom - outRect->top - 14) / 2;
	InsetRect(outRect, 0, heightToRemove);
}

void CalculateThumbRect(short varCode, ControlHandle theControl, Rect *outRect) {
	Rect track;
	short availableWidth = 0;
	float xPos = 0;
	short valueWidth = (**theControl).contrlMax - (**theControl).contrlMin;
	CalculateTrackRect(varCode, theControl, &track);
	*outRect = (**theControl).contrlRect;
	outRect->right = outRect->left + 16;
	
	availableWidth = track.right - track.left - (outRect->right - outRect->left);
	xPos = (((float)availableWidth) / ((float)valueWidth)) * ((float)(**theControl).contrlValue);
	
	outRect->left += (short) xPos;
	outRect->right += (short) xPos;
}

short CalculateValueFromThumbLeft(short varCode, ControlHandle theControl, short thumbLeft) {
	Rect track;
	Rect thumb;
	short relativeThumbLeft = 0;
	short availableWidth = 0;
	float xPos = 0;
	short valueWidth = (**theControl).contrlMax - (**theControl).contrlMin;
	CalculateTrackRect(varCode, theControl, &track);
	relativeThumbLeft = thumbLeft - track.left;
	CalculateThumbRect(varCode, theControl, &thumb);
	
	availableWidth = track.right - track.left - (thumb.right - thumb.left);
	return (((float)availableWidth) / ((float)valueWidth)) / relativeThumbLeft;
}


void HandleDraw(short varCode, ControlHandle theControl, short partCode) {
	static Pattern gray = { { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 } };
	static Pattern black = { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
	Rect track;
	Rect thumb;
	Boolean drawInactive = (**theControl).contrlHilite == kControlDisabledPart || (**theControl).contrlHilite == kControlInactivePart;

	CalculateTrackRect(varCode, theControl, &track);
	CalculateThumbRect(varCode, theControl, &thumb);
	
	ForeColor(whiteColor);
	PaintRoundRect(&track, 12, 12);
	ForeColor(blackColor);
	if (drawInactive) {
		PenPat(&gray);
	}
	FrameRoundRect(&track, 12, 12);

	InsetRect(&track, 4, 4);
	if (!drawInactive) {
		PenPat(&gray);
		PaintRoundRect(&track, 6, 6);
		PenPat(&black);
	}
	FrameRoundRect(&track, 6, 6);
	
	ForeColor(whiteColor);
	PaintRoundRect(&thumb, 11, 11);
	
	ForeColor(blackColor);
	PenPat(&gray);
	MoveTo(thumb.left + 3, thumb.top);
	LineTo(thumb.left + 3, thumb.bottom - 1);
	MoveTo(thumb.right - 3 - 1, thumb.top);
	LineTo(thumb.right - 3 - 1, thumb.bottom - 1);
	if (!drawInactive) {
		PenPat(&black);
	}
	
	FrameRoundRect(&thumb, 10, 10);
	
	if (drawInactive) {
		PenPat(&black); // Restore opaque pen.
	}

}

ControlPartCode HandleTest(short varCode, ControlHandle theControl, Point *pos) {
	if (PtInRect(*pos, &(**theControl).contrlRect)) {
		Rect track;
		Rect thumb;

		CalculateTrackRect(varCode, theControl, &track);
		CalculateThumbRect(varCode, theControl, &thumb);
		
		if (PtInRect(*pos, &thumb)) {
			return kControlIndicatorPart;
		} else if (PtInRect(*pos, &track)) {
			if (pos->h <= thumb.left) {
				return kControlPageUpPart;
			} else if (pos->h >= thumb.right) {
				return kControlPageDownPart;
			}
		} 
		return kControlButtonPart;
	}
	
	return kControlNoPart;
}

void HandleCalculateControlRegion(short varCode, ControlHandle theControl, RgnHandle destRegion) {
	RectRgn(destRegion, &(**theControl).contrlRect);
}

void HandleCalculateIndicatorRegion(short varCode, ControlHandle theControl, RgnHandle destRegion) {
	Rect thumb;
	CalculateThumbRect(varCode, theControl, &thumb);
	
	OpenRgn();
	FrameRoundRect(&thumb, 11, 11);
	CloseRgn(destRegion);
}

void HandleInit(short varCode, ControlHandle theControl) {
	// (**theControl).contrlData is for our custom data.
}

void HandleDispose(short varCode, ControlHandle theControl) {
	// (**theControl).contrlData is for our custom data.
}

ControlPartCode HandleMoveIndicator(short varCode, ControlHandle theControl, Point *pos) {
	Rect thumb;
	CalculateThumbRect(varCode, theControl, &thumb);
	
	(**theControl).contrlValue = CalculateValueFromThumbLeft(varCode, theControl, thumb.left + pos->h);
}

void HandleCalcThumbDragInfo(short varCode, ControlHandle theControl, Point *clickPos, IndicatorDragConstraint *dragInfo) {
	Rect thumb;
	Rect track;
	CalculateThumbRect(varCode, theControl, &track);
	CalculateThumbRect(varCode, theControl, &thumb);

	dragInfo->limitRect = track;
	dragInfo->limitRect.top = thumb.top;
	dragInfo->limitRect.bottom = thumb.bottom;
	
	dragInfo->slopRect = dragInfo->limitRect;
	InsetRect(&dragInfo->slopRect, 100, 100);
	
	dragInfo->axis = hAxisOnly;
}

void HandleAutoTrack(short varCode, ControlHandle theControl, short partCode) {

}


pascal long main(short varCode, ControlHandle theControl, short message, long param) {
	long result = 0;
	EnterCodeResource();
	
	switch (message) {
		case drawCntl:
			if ((**theControl).contrlVis) {
				HandleDraw(varCode, theControl, LoWord(param));
			}
			break;

		case testCntl:
			if ((**theControl).contrlVis && (**theControl).contrlHilite != kControlInactivePart) {
				result = HandleTest(varCode, theControl, (Point*) &param);
			} else {
				result = kControlNoPart;
			}
			break;

		case calcCRgns: {
			UInt32 cleanParam = (UInt32) param;
			if (cleanParam & 0x80000000L) {
				cleanParam &= 0x7FFFFFFFL;
				HandleCalculateIndicatorRegion(varCode, theControl, (RgnHandle) cleanParam);
			} else {
				HandleCalculateControlRegion(varCode, theControl, (RgnHandle) cleanParam);
			}
			break;
		}
		
		case initCntl:
			HandleInit(varCode, theControl);
			break;

		case dispCntl:
			HandleDispose(varCode, theControl);
			break;

		case posCntl:
			HandleMoveIndicator(varCode, theControl, (Point*) &param);
			break;

		case thumbCntl: {
			IndicatorDragConstraint *dragInfo = (IndicatorDragConstraint*) param;
			Point *clickPos = (Point*) &dragInfo->limitRect.top;
			HandleCalcThumbDragInfo(varCode, theControl, clickPos, dragInfo);
			break;
		}
		
		case dragCntl:
			return 0; // Get default control tracking with a grey region.
			break;

		case autoTrack:
			HandleAutoTrack(varCode, theControl, LoWord(param));
			break;

		case calcCntlRgn:
			HandleCalculateControlRegion(varCode, theControl, (RgnHandle) param);
			break;

		case calcThumbRgn:
			HandleCalculateIndicatorRegion(varCode, theControl, (RgnHandle) param);
			break;
	}
	
	ExitCodeResource();
	
	return result;
}