#pragma once#include "CWindowAttachment.h"#include <Lists.h>class CListManagerWindowAttachment : public CWindowAttachment {public:	CListManagerWindowAttachment(CWindow *wd, Rect *box);	~CListManagerWindowAttachment();		virtual void Draw();		virtual bool HandleMouseDown(const EventRecord &event);		virtual void ResizedWindowFrom(Point oldSize);		virtual void Activate();	virtual void Deactivate();	protected:	void InvalidateListRect();	ListHandle mList;};