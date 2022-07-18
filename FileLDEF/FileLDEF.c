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
			
		case lDrawMsg:
			if (dataLen == 0) {
				return;
			}
			{
			RgnHandle oldClip = NewRgn();
			PenState state = {};
			struct FileEntry data = {};
			SInt8 theListSvState = 0;
			SInt8 cellsSvState = 0;
			Handle cells = NULL;
			struct FileEntry *srcData = NULL;
			Rect iconRect = *cellRect;
			short topOffset = 0;
			
			theListSvState = HGetState((Handle)theList);
			HLock((Handle)theList);
			
			cells = (**theList).cells;
			cellsSvState = HGetState(cells);
			HLock(cells);

			srcData = (struct FileEntry *) ((*cells) + dataOffset);
			topOffset = (cellRect->bottom - cellRect->top - 32) / 2;
			
			BlockMove(srcData, &data, sizeof(data));
			
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
				HLock(data.icon);
				PlotIcon(&iconRect, data.icon);
				HUnlock(data.icon);
			}
			
			if (selected) {
				LMSetHiliteMode(LMGetHiliteMode() & ~(1 << 7));
				InvertRect(cellRect);
			}
			
			HSetState((**theList).cells, cellsSvState);
			HSetState((Handle)theList, theListSvState);
			
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