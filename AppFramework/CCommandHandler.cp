#include "CCommandHandler.h"
#include "Window.h"
#include <Memory.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Events.h>
#include <Menus.h>
#include <Windows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <SegLoad.h>

void CCommandHandler::HandleCommand(OSType command, short menuID, short itemIndex) {
	if (mNextHandler) {
		mNextHandler->HandleCommand(command, menuID, itemIndex);
	} else {
		SysBeep(007);
	}
}

void CCommandHandler::UpdateCommand(OSType command, short menuID, short itemIndex) {
	if (mNextHandler) {
		mNextHandler->UpdateCommand(command, menuID, itemIndex);
	} else {
		DisableItem(GetMenuHandle(menuID), itemIndex);
	}
}

