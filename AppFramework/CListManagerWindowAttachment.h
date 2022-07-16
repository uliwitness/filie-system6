#pragma once#include "CWindowAttachment.h"#include <Lists.h>class CListManagerWindowAttachment : public CWindowAttachment {public:	CListManagerWindowAttachment(CWindow *wd, Rect *box, bool leaveGrowRoom = false, short ldefID = 0);	~CListManagerWindowAttachment();		void AddRow(const void* data, short length);		virtual void Draw();		virtual bool HandleMouseDown(const EventRecord &event);		virtual void ResizedWindowFrom(Point oldSize);		virtual void Activate();	virtual void Deactivate();	protected:	void InvalidateListRect();	ListHandle mList;};