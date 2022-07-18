#include <Files.h>

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

struct FileEntry {
	FSSpec file;
	Handle icon;
};

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif
