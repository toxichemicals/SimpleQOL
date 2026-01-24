#ifndef SimpleConflictsFixer
#define SimpleConflictsFixer

#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600
#define CloseWindow WinCloseWindow
#define ShowCursor WinShowCursor
#define Rectangle WinRectangle
#define DrawText WinDrawText
#include <winsock2.h>
#include <windows.h>
#include <objbase.h>
#undef CloseWindow
#undef ShowCursor
#undef Rectangle
#undef DrawText
#ifdef interface
#undef interface
#endif
#endif

#endif
