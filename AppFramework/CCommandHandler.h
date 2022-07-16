#pragma once

#include <Types.h>

class CCommandHandler {
public:
	CCommandHandler(CCommandHandler *nextHandler) : mNextHandler(nextHandler) {}
	virtual ~CCommandHandler() {}

	virtual void HandleCommand(OSType command, short menuID, short itemIndex);
	virtual void UpdateCommand(OSType command, short menuID, short itemIndex);
	
protected:
	CCommandHandler *mNextHandler;
};