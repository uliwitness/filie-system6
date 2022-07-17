#include "CApplication.h"
#include "CWindow.h"
#include "CDialog.h"
#include "ProjectDefines.h"
#include "SMenuCommands.h"
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
#include <Devices.h>
#include <stdio.h>
#include <SIOUX.h>
#include "SIOUXGlobals.h"

CApplication	*gApplication = NULL;


static short ToolboxTrapTableSize(void)
{
	if (GetToolTrapAddress(_InitGraf) == GetToolTrapAddress(0xAA6E)) {
		return 0x0200;
	} else {
		return 0x0400;
	}
}


static Boolean IsTrapAvailable(short theTrap)
{
	TrapType		trapType;
	Boolean			available;

	if ((theTrap & 0x0800) > 0) {
		trapType = ToolTrap;
	} else {
		trapType = OSTrap;
	}
	if (trapType == ToolTrap) {
		theTrap &= 0x07FF;
		if (theTrap >= ToolboxTrapTableSize()) {
			theTrap = _Unimplemented;
		}
	}
	available = NGetTrapAddress(theTrap, trapType)
				!= GetToolTrapAddress(_Unimplemented);
	return available;
}


extern "C" void main(void) {
	gApplication = new CApplication();
	gApplication->SetDelegate(new APPLICATION_DELEGATE_CLASS(gApplication));
	gApplication->Run();
}


CApplication* CApplication::Singleton() {
	return gApplication;
}

Boolean CApplication::DequeueOneEvent(EventMask eventMask, EventRecord *outEvent) {
	if (mHaveWaitNextEvent) {
		return WaitNextEvent(eventMask, outEvent, 60, NULL);
	} else {
		SystemTask();
		return GetNextEvent(eventMask, outEvent);
	}
}

void CApplication::Run() {
	Initialize();
	
	mDelegate->StartUp();
	
	SIOUXSettings.initializeTB = false;
	SIOUXSettings.standalone = false;
	SIOUXSettings.setupmenus = false;
	SIOUXSettings.autocloseonquit = true;
	SIOUXSettings.asktosaveonclose = false;
	
	if (!mHaveAppleEvents) {
		short request = 0, count = 0;
		CountAppFiles(&request, &count);
		printf("Number of files to open: %d (job %d)\n", (int)count, (int)request);
		if (count == 0) {
			mDelegate->OpenUntitledDocument();
		} else {
			for(short x = 1; x <= count; ++x) {
				AppFile appFile = {};
				GetAppFiles(x, &appFile);
				CFileSpec file(appFile.vRefNum, 0, appFile.fName);
				mDelegate->OpenDocument(&file);
				ClrAppFiles(x);
			}
		}
	}
	
	while (!mQuit) {
		if (DequeueOneEvent(everyEvent, &mCurrentEvent)) {
			if (SIOUXHandleOneEvent(&mCurrentEvent)) {
				continue;
			}
			if (IsDialogEvent(&mCurrentEvent)) {
				DialogPtr dialog = NULL;
				short itemIndex = 0;
				if (DialogSelect(&mCurrentEvent, &dialog, &itemIndex)) {
					CDialog * dialogObj = (CDialog*) CWindow::FromMac(dialog);
					dialogObj->HandleItemClick(itemIndex);
				}
				continue;
			}
			switch (mCurrentEvent.what) {
				case mouseDown: {
					WindowPtr clickedWindow = NULL;
					short part = FindWindow(mCurrentEvent.where, &clickedWindow);
					if (clickedWindow != NULL && clickedWindow == WindowPtr(SIOUXTextWindow)) {
						continue; // Should never get here, but sometimes SIOUXHandleOneEvent() seems to slip.
					}
					switch (part) {
						case inMenuBar: {
							UpdateMenuItems();
							long menuChoice = MenuSelect(mCurrentEvent.where);
							if (menuChoice != 0) {
								short menuID = HiWord(menuChoice);
								short itemIndex = LoWord(menuChoice);
								SendMenuChoice(menuID, itemIndex);
								HiliteMenu(0);
							}
							break;
						}
						
						case inSysWindow:
							SystemClick(&mCurrentEvent, clickedWindow);
							break;
						
						case inContent:
							CWindow::FromMac(clickedWindow)->HandleMouseDown(mCurrentEvent);
							break;
						
						case inDrag: {
							CWindow * windowObject = CWindow::FromMac(clickedWindow);
							windowObject->HandleDrag(mCurrentEvent);
							break;
						}
						
						case inGrow:
							CWindow::FromMac(clickedWindow)->HandleResize(mCurrentEvent);
							break;
						
						case inGoAway:
							CWindow::FromMac(clickedWindow)->HandleClose(mCurrentEvent);
							break;
						
						case inZoomIn:
						case inZoomOut:
							CWindow::FromMac(clickedWindow)->HandleZoom(mCurrentEvent, part);
							break;
					}
					break;
				}
				
				case mouseUp: {
					WindowPtr clickedWindow = NULL;
					short part = FindWindow(mCurrentEvent.where, &clickedWindow);
					if (clickedWindow != NULL && clickedWindow == WindowPtr(SIOUXTextWindow)) {
						continue; // Should never get here, but sometimes SIOUXHandleOneEvent() seems to slip.
					}
					switch (part) {
						case inMenuBar:
							break;
						
						case inSysWindow:
							SystemClick(&mCurrentEvent, clickedWindow);
							break;
						
						case inContent:
							CWindow::FromMac(clickedWindow)->HandleMouseUp(mCurrentEvent);
							break;
						
						case inDrag:
						case inGrow:
						case inGoAway:
						case inZoomIn:
						case inZoomOut:
							break;
					}
					break;
				}
				
				case keyDown:
				case autoKey:
					if ((mCurrentEvent.modifiers & cmdKey) != 0) {
						UpdateMenuItems();
						long menuChoice = MenuKey(char(mCurrentEvent.message & charCodeMask));
						if (menuChoice != 0) {
							short menuID = HiWord(menuChoice);
							short itemIndex = LoWord(menuChoice);
							HiliteMenu(menuID);
							DrawMenuBar();
							SendMenuChoice(menuID, itemIndex);
							long outTicks = 0;
							Delay(10, &outTicks);
							HiliteMenu(0);
							break;
						}
					}
					break;
				
				case activateEvt: {
					WindowPtr window = (WindowPtr)mCurrentEvent.message;
					if (window == WindowPtr(SIOUXTextWindow)) {
						continue; // Should never get here, but sometimes SIOUXHandleOneEvent() seems to slip.
					}
					Boolean activate = (mCurrentEvent.modifiers & activeFlag) != 0;
					if (activate) {
						CWindow::FromMac(window)->Activate();
					} else {
						CWindow::FromMac(window)->Deactivate();
					}
					break;
				}
				
				case updateEvt: {
					WindowPtr window = (WindowPtr)mCurrentEvent.message;
					if (window == WindowPtr(SIOUXTextWindow)) {
						continue; // Should never get here, but sometimes SIOUXHandleOneEvent() seems to slip.
					}
					CWindow::FromMac(window)->HandleUpdate(mCurrentEvent);
					break;
				}
				
				case diskEvt: {
					OSErr err = HiWord(mCurrentEvent.message);
					if (err != noErr) {
						DILoad();
						Point pos = {120, 120};
						err = DIBadMount(pos, mCurrentEvent.message);
						DIUnload();
					}
					break;
				}
				
				case osEvt:
					long osEvtType = (mCurrentEvent.message & 0xFF000000) >> 24;
					switch(osEvtType) {
						case suspendResumeMessage: {
							CWindow * frontWindowObj = CWindow::Frontmost();
							if (frontWindowObj) {
								if (mCurrentEvent.message & resumeFlag) {
									frontWindowObj->Activate();
								} else {
									frontWindowObj->Deactivate();
								}
							}
							break;
						}
						case mouseMovedMessage:
							break;
					}
					break;
				
				case kHighLevelEvent: {
					OSErr err = AEProcessAppleEvent(&mCurrentEvent);
					if (err != noErr) {
						printf("Error processing Apple Event: %d\n", (int)err);
					}
					break;
				}
			}
		}
	}
}

pascal OSErr CApplication::HandleODoc(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon) {
	CApplication *self = (CApplication*)handlerRefcon;
	AEDescList docList = {};
	long itemsInList = 0;
	AEKeyword keywd = 0;
	FSSpec myFSS = {};
	Size actualSize = 0;
	DescType returnedType = 0;
	
	printf("Received 'odoc' event\n");
	
	OSErr myErr = AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &docList);
	if (myErr == noErr) {
		myErr = AECountItems(&docList, &itemsInList);
		if (myErr == noErr) {
			printf("Asked to open %ld files\n", itemsInList);
			for(long x = 1; x <= itemsInList; ++x) {
				myErr = AEGetNthPtr(&docList, x, typeFSS, &keywd, &returnedType, &myFSS, sizeof(myFSS), &actualSize);
				CFileSpec specToOpen(&myFSS);
				self->mDelegate->OpenDocument(&specToOpen);
			}
		}
		
		AEDisposeDesc(&docList);
	}
	
	return myErr;
}

pascal OSErr CApplication::HandleOApp(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon) {
	CApplication *self = (CApplication*)handlerRefcon;

	self->mDelegate->OpenUntitledDocument();
	
	return noErr;
}


void CApplication::Initialize() {
	MaxApplZone();

	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();

	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();

	GetDateTime((unsigned long*) &qd.randSeed);
	
	Handle menuBar = GetNewMBar(128);
	SetMenuBar(menuBar);
	DisposeHandle(menuBar);
	AppendResMenu(GetMenuHandle(128), 'DRVR');
	DrawMenuBar();
	
	// Check if Apple Events are available (because on OS 9 GetAppFiles is unavailable):
	long aeStatus = 0;
	mHaveAppleEvents = (Gestalt(gestaltAppleEventsAttr, &aeStatus) == noErr
						&& (aeStatus & (1 << gestaltAppleEventsPresent)) != 0);
	
	if (mHaveAppleEvents) {
		OSErr err = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, HandleODoc, (long) this, false);
		if (err != noErr) {
			printf("Error installing 'odoc' event handler: %d\n", err);
		}
		err = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, HandleOApp, (long) this, false);
		if (err != noErr) {
			printf("Error installing 'oapp' event handler: %d\n", err);
		}
	}
	
	// Check if WaitNextEvent is available: (requires MultiFinder/System 7)
	mHaveWaitNextEvent = IsTrapAvailable(_WaitNextEvent);
}

void CApplication::UpdateMenuItems() {
	CCommandHandler * commandHandler = mDelegate;
	CWindow * frontWindowObj = CWindow::Frontmost();
	if (frontWindowObj != NULL) {
		commandHandler = frontWindowObj;
	}
	
	for (short menuID = 128; ; ++menuID) {
		MenuHandle currMenu = GetMenuHandle(menuID);
		if (!currMenu) {
			break;
		}
		OSType command = 0;
		SMenuCommandsHandle commands = (SMenuCommandsHandle) GetResource('MNUC', menuID);
		if (commands) {
			short numItems = CountMItems(currMenu);
			short numCommands = (**commands).count;
			for (short itemIndex = 1; itemIndex <= numItems; ++itemIndex) {
				if (itemIndex <= numCommands) {
					command = (**commands).commands[itemIndex - 1];
				} else {
					command = 0L;
				}
				commandHandler->UpdateCommand(command, menuID, itemIndex);
			}
		}
	}
}


void CApplication::SendMenuChoice(short menuID, short itemIndex) {
	OSType command = 0;
	SMenuCommandsHandle commands = (SMenuCommandsHandle) GetResource('MNUC', menuID);
	if (commands) {
		if (itemIndex <= (**commands).count) {
			command = (**commands).commands[itemIndex - 1];
		}
	}
	
	CWindow * frontWindowObj = CWindow::Frontmost();
	if (frontWindowObj) {
		frontWindowObj->HandleCommand(command, menuID, itemIndex);
	} else {
		mDelegate->HandleCommand(command, menuID, itemIndex);
	}
}

void CApplication::HandleCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'quit') {
		if (mDelegate->ShouldShutDown()) {
			mQuit = true;
		}
	} else if (command == 'new ') {
		mDelegate->OpenUntitledDocument();
	} else if (command == 0L && menuID == 128) {
		Str255 deskAccName = {};
		GetMenuItemText(GetMenuHandle(menuID), itemIndex, deskAccName);
		OpenDeskAcc(deskAccName);
	} else {
		CCommandHandler::HandleCommand(command, menuID, itemIndex);
	}
}

void CApplication::UpdateCommand(OSType command, short menuID, short itemIndex) {
	if (command == 'quit') {
		EnableItem(GetMenuHandle(menuID), itemIndex);
	} else if (command == 'new ') {
		EnableItem(GetMenuHandle(menuID), itemIndex);
	} else if (command == 0L && menuID == 128) {
		EnableItem(GetMenuHandle(menuID), itemIndex);
	} else {
		CCommandHandler::UpdateCommand(command, menuID, itemIndex);
	}
}
