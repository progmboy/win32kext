// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here


#ifdef _WIN64
#define KDEXT_64BIT
#else
#define KDEXT_32BIT
#endif

#include <wdbgexts.h>
#include <dbgeng.h>

#pragma comment(lib, "dbgeng.lib")

#ifdef _DEBUG
# define DEBUGPRINT dprintf
#else
# define DEBUGPRINT(...) __noop
#endif

#ifdef WIN32KEXT_EXPORTS
#define WIN32KEXT_API __declspec(dllexport)
#else
#define WIN32KEXT_API __declspec(dllimport)
#endif


//
// windbg pointer is 64-bit
//

typedef ULONG64 WDBG_PTR, *PWDBG_PTR;
