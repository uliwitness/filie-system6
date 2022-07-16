#include <Controls.h>
#include <QuickDraw.h>
#include <SetUpA4.h>
#include <A4Stuff.h>
#include <math.h>
#include "FileLDEF.h"

pascal void main(short message, Boolean selected, Rect *cellRect, Cell theCell, short dataOffset,
				 short dataLen, ListHandle theList) {
	EnterCodeResource();
	
	switch (message) {
		case lInitMsg:
			break;
			
		case lDrawMsg: {
			RgnHandle oldClip = NewRgn();
			FSSpec spec = {};
			PenState state = {};
			
			GetClip(oldClip);
			GetPenState(&state);
			ClipRect(cellRect);
			
			ForeColor(whiteColor);
			PaintRect(cellRect);
			ForeColor(blackColor);
			BlockMove((*(**theList).cells) + dataOffset, &spec, sizeof(FSSpec));
			MoveTo(cellRect->left + 4, cellRect->bottom - 4);
			DrawString(spec.name);
			
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