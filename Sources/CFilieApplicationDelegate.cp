#include "CFilieApplicationDelegate.h"
#include "CFilieWindow.h"
#include "CDialog.h"

CFilieApplicationDelegate::CFilieApplicationDelegate(CCommandHandler* parent)
: CApplicationDelegate(parent) {

}

void CFilieApplicationDelegate::OpenUntitledDocument() {
	Rect windRect = { 100, 100, 580, 740 };
	CWindow *window = new CFilieWindow(&windRect);
	window->CreateWindow();
	window->SetUniqueTitle("\pMy Computer");
}

void CFilieApplicationDelegate::OpenDocument(CFileSpec *fileOrFolder) {
	Rect windRect = { 100, 100, 580, 740 };
	CWindow *window = new CFilieWindow(&windRect, fileOrFolder);
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
