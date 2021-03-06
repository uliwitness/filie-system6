#include <Controls.h>
#include <QuickDraw.h>

void CalculateTrackRect(short varCode, ControlHandle theControl, Rect *outRect);
void CalculateThumbRect(short varCode, ControlHandle theControl, Rect *outRect);
short CalculateValueFromThumbLeft(short varCode, ControlHandle theControl, short thumbLeft);

void HandleDraw(short varCode, ControlHandle theControl, short partCode);
ControlPartCode HandleTest(short varCode, ControlHandle theControl, Point *pos);

void HandleCalculateControlRegion(short varCode, ControlHandle theControl, RgnHandle destRegion);
void HandleCalculateIndicatorRegion(short varCode, ControlHandle theControl, RgnHandle destRegion);

void HandleInit(short varCode, ControlHandle theControl);
void HandleDispose(short varCode, ControlHandle theControl);

ControlPartCode HandleMoveIndicator(short varCode, ControlHandle theControl, Point *pos);
void HandleCalcThumbDragInfo(short varCode, ControlHandle theControl, Point *clickPos, IndicatorDragConstraint *dragInfo);
void HandleAutoTrack(short varCode, ControlHandle theControl, short partCode);

pascal long main(short varCode, ControlHandle theControl, short message, long param);
