#include <Controls.h>
#include <QuickDraw.h>
#include <SetUpA4.h>
#include <A4Stuff.h>
#include <math.h>
#include "FileLDEF.h"
#include "FileLDEFData.h"

pascal void main(short message, Boolean selected, Rect *cellRect, Cell theCell, short dataOffset,
				 short dataLen, ListHandle theList) {
	EnterCodeResource();
	
	switch (message) {
		case lInitMsg:
			if ((**theList).cellSize.v < 34) {
				(**theList).cellSize.v = 34;
			}
			break;
			
		case lDrawMsg: {
			RgnHandle oldClip = NewRgn();
			PenState state = {};
			struct FileEntry data = {};
			Rect iconRect = *cellRect;
			short topOffset = (cellRect->bottom - cellRect->top - 32) / 2;
			
			BlockMove((*(**theList).cells) + dataOffset, &data, sizeof(data));
			
			iconRect.left += 4;
			iconRect.right = iconRect.left + 32;
			iconRect.top += topOffset;
			iconRect.bottom = iconRect.top + 32;
			GetClip(oldClip);
			GetPenState(&state);
			ClipRect(cellRect);
			
			ForeColor(whiteColor);
			PaintRect(cellRect);
			ForeColor(blackColor);
			MoveTo(iconRect.right + 4, cellRect->bottom - 4);
			DrawString(data.file.name);
			if (data.icon) {
				PlotIconHandle(&iconRect, atNone, ttNone, data.icon);
			}
			
			if (selected) {
				LMSetHiliteMode(LMGetHiliteMode() & ~(1 << 7));
				InvertRect(cellRect);
			}
			
			SetClip(oldClip);
			DisposeRgn(oldClip);
			SetPenState(&state);
			break;
		}
		
		case lHiliteMsg: {
			RgnHandle oldClip = NewRgn();
			PenState state = {};
			
			GetClip(oldClip);
			GetPenState(&state);
			ClipRect(cellRect);
			
			//PenMode(patCopy + hilite);
			LMSetHiliteMode(LMGetHiliteMode() & ~(1 << 7));
			InvertRect(cellRect);
			//PenMode(patOr);
			
			SetClip(oldClip);
			DisposeRgn(oldClip);
			SetPenState(&state);
			break;
		}
			
		case lCloseMsg:
			break;
	}
	
	ExitCodeResource();
}