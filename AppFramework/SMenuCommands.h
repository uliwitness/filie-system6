#pragma once

struct SMenuCommands {
	short count;
	OSType commands[];
};
typedef SMenuCommands **SMenuCommandsHandle;
