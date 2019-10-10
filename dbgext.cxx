
#include "stdafx.h"
#include "dbgext.hpp"


PDEBUG_CLIENT4        g_ExtClient;
PDEBUG_CONTROL4       g_ExtControl;
PDEBUG_SYMBOLS2       g_ExtSymbols;

WINDBG_EXTENSION_APIS   ExtensionApis;

ULONG   TargetMachine;
BOOL    Connected;

// Queries for all debugger interfaces.
extern "C" HRESULT
ExtQuery(PDEBUG_CLIENT4 Client)
{
	HRESULT Status;

	if ((Status = Client->QueryInterface(__uuidof(IDebugControl4),
		(void**)&g_ExtControl)) != S_OK)
	{
		goto Fail;
	}
	if ((Status = Client->QueryInterface(__uuidof(IDebugSymbols2),
		(void**)&g_ExtSymbols)) != S_OK)
	{
		goto Fail;
	}
	g_ExtClient = Client;

	return S_OK;

Fail:
	ExtRelease();
	return Status;
}

// Cleans up all debugger interfaces.
void
ExtRelease(void)
{
	g_ExtClient = NULL;
	EXT_RELEASE(g_ExtControl);
	EXT_RELEASE(g_ExtSymbols);
}


// Normal output.
void __cdecl
ExtOut(PCSTR Format, ...)
{
	va_list Args;

	va_start(Args, Format);
	g_ExtControl->OutputVaList(DEBUG_OUTPUT_NORMAL, Format, Args);
	va_end(Args);
}

// Error output.
void __cdecl
ExtErr(PCSTR Format, ...)
{
	va_list Args;

	va_start(Args, Format);
	g_ExtControl->OutputVaList(DEBUG_OUTPUT_ERROR, Format, Args);
	va_end(Args);
}

// Warning output.
void __cdecl
ExtWarn(PCSTR Format, ...)
{
	va_list Args;

	va_start(Args, Format);
	g_ExtControl->OutputVaList(DEBUG_OUTPUT_WARNING, Format, Args);
	va_end(Args);
}

void __cdecl
ExtOutDml(PCSTR Format, ...)
{
	va_list Args;

	va_start(Args, Format);
	g_ExtControl->ControlledOutputVaList(DEBUG_OUTCTL_DML, DEBUG_OUTPUT_NORMAL, Format, Args);
	va_end(Args);
}

EXTERN_C
WIN32KEXT_API
HRESULT
CALLBACK
DebugExtensionInitialize(
	OUT PULONG Version,
	OUT PULONG Flags)
{
	IDebugClient* DebugClient;
	PDEBUG_CONTROL4 DebugControl;
	HRESULT hr = S_OK;

	*Version = DEBUG_EXTENSION_VERSION(1, 0);
	*Flags = 0;

	//
	// Create a new debugclient
	//

	if ((hr = DebugCreate(__uuidof(IDebugClient), (void**)&DebugClient)) != S_OK) {
		return hr;
	}

	if ((hr = DebugClient->QueryInterface(__uuidof(IDebugControl),
		(void**)&DebugControl)) == S_OK){

		//
		// Get the windbg-style extension APIS
		//

		ExtensionApis.nSize = sizeof(ExtensionApis);
		hr = DebugControl->GetWindbgExtensionApis64(&ExtensionApis);

		//
		// check is in kernel mode
		//

		ULONG dbgClass, dbgQualifier;
		if (SUCCEEDED(DebugControl->GetDebuggeeType(&dbgClass, &dbgQualifier))) {
			if (dbgClass != DEBUG_CLASS_KERNEL) {
				dprintf("This extension is for kernel-mode only\n");
				hr = S_FALSE;
			}
		}

		//
		// check os
		//

		ULONG PlatformId, Win32Major, Win32Minor, KdMajor, KdMinor;
		if (SUCCEEDED(DebugControl->GetSystemVersionValues(&PlatformId, &Win32Major, &Win32Minor,
			&KdMajor, &KdMinor))) {
			DEBUGPRINT("OS Detected: %d.%d.%d.%d\n", Win32Major, Win32Minor, KdMajor, KdMinor);
		}

		DebugControl->Release();

	}
	DebugClient->Release();
	return hr;
}

EXTERN_C
WIN32KEXT_API
void
CALLBACK
DebugExtensionUninitialize(void)
{
	//DEBUGPRINT("DebugExtensionUninitialize\n");
	return;
}