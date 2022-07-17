#include "CListManagerWindowAttachment.h"
#include "CWindow.h"
#include "StPortChanger.h"
#include <stdio.h>
#include <string.h>


CListManagerWindowAttachment::CListManagerWindowAttachment(CWindow *wd, Rect *box, short ldefID)
: CWindowAttachment(wd) {
	WindowPtr window = wd->ToMac();
	StPortChanger portChanger(window);
	
	const char* str = NULL;
	Rect bodyBox = *box;
	bodyBox.right -= 15; // Account for scroll bar.
	Rect dataBounds = { 0, 0, 0, 1};
	Point cellSize = { 0, 0 };
	Point cell = { 0, 0 };
	mList = LNew(&bodyBox, &dataBounds, cellSize, ldefID, window, true, true, false, true);
}

CListManagerWindowAttachment::~CListManagerWindowAttachment() {
	LDispose(mList);
	mList = NULL;
}


void CListManagerWindowAttachment::AddRow(const void* data, short length) {
	Point cell = { 0, 0 };
	cell.v = (**mList).dataBounds.bottom;
	LAddRow(1, cell.v, mList);
	LSetCell(data, length, cell, mList);
}


void CListManagerWindowAttachment::GetSelectedRow(void* outData, short *ioLength) {
	Cell selCell = {};
	if (LGetSelect(true, &selCell, mList)) {
		LGetCell(outData, ioLength, selCell, mList);
	} else {
		*ioLength = 0;
	}
}


void CListManagerWindowAttachment::Draw() {
	ForeColor(whiteColor);
	PaintRect(&(**mList).rView);
	
	ForeColor(blackColor);
	LUpdate((**mList).port->visRgn, mList);
	
	Rect borderBox = (**mList).rView;
	InsetRect(&borderBox, -1, -1);
	FrameRect(&borderBox);
}

bool CListManagerWindowAttachment::HandleMouseDown(const EventRecord &event) {
	Point pos = event.where;
	mWindow->GlobalToLocalPoint(&pos);
	Rect box = (**mList).rView;
	box.right += 15;
	if (!PtInRect(pos, &box)) {
		return false;
	}
	
	if (LClick(pos, event.modifiers, mList)) {
		HandleCommand(mCommand, 0, 0);
	}
	return true;
}

void CListManagerWindowAttachment::ResizedWindowFrom(Point oldSize) {
	InvalidateListRect();
	
	Rect box = (**mList).rView;
	ResizedFromOldToNewWindowRect(&box, oldSize, &(**mList).port->portRect);

	LSize(box.right - box.left, box.bottom - box.top, mList);
	(**mList).rView = box;
	
	(**mList).cellSize.h = box.right - box.left;
	
	InvalidateListRect();
}

void CListManagerWindowAttachment::InvalidateListRect() {
	StPortChanger portChanger(mWindow->ToMac());
	Rect box = (**mList).rView;
	InsetRect(&box, -1, -1);
	box.right += 15;
	InvalRect(&box);
}
	
void CListManagerWindowAttachment::Activate() {
	LActivate(true, mList);
}

void CListManagerWindowAttachment::Deactivate() {
	LActivate(false, mList);
}
