#ifndef SimpleConflictsFixer
#define SimpleConflictsFixer

#ifdef _WIN32
// 1. Fix the Version Redefinition Warning
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600

// 2. The Renaming Hack
// We tell the compiler: "When you see CloseWindow in the NEXT file,
// pretend it's named WinCloseWindow"
#define CloseWindow WinCloseWindow
#define ShowCursor WinShowCursor
#define Rectangle WinRectangle
#define DrawText WinDrawText

// 3. Include Windows headers (Winsock2 MUST come before Windows.h)
#include <winsock2.h>
#include <windows.h>
#include <objbase.h>

// 4. Clean up
// Now we stop the renaming so that when YOUR code says 'CloseWindow',
// it correctly points back to Raylib's version.
#undef CloseWindow
#undef ShowCursor
#undef Rectangle
#undef DrawText

// Fix the 'interface' macro that breaks socket functions
#ifdef interface
#undef interface
#endif
#endif

#endif
