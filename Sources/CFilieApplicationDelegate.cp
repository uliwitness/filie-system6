#include "CFilieApplicationDelegate.h"
#include "CFilieWindow.h"
#include "CDialog.h"
#include "ResourceIDs.h"

CFilieApplicationDelegate::CFilieApplicationDelegate(CCommandHandler* parent)
: CApplicationDelegate(parent) {

}

void CFilieApplicationDelegate::OpenUntitledDocument() {
	CWindow *window = new CFilieWindow();
	window->CreateWindow();
	
	Str255 wTitle;
	GetIndString(wTitle, kFilieWindowStrings, kFilieWindowStringsMyComputer);
	window->SetUniqueTitle(wTitle);
}

void CFilieApplicationDelegate::OpenDocument(CFileSpec *fileOrFolder) {
	CWindow *window = new CFilieWindow(fileOrFolder);
	window->CreateWindow();
	window->SetUniqueTitle(fileOrFolder->name);
}

void CFilieApplicationDelegate::HandleCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'abou') {
		CDialog * aboutWindow = new CDialog(128);
		aboutWindow->CreateWindow();
	} else {
		mNextHandler->HandleCommand(command, menuID, itemIndex);
	}
}

void CFilieApplicationDelegate::UpdateCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'abou') {
		EnableItem(GetMenuHandle(menuID), itemIndex);
	} else {
		CCommandHandler::UpdateCommand(command, menuID, itemIndex);
	}
}
