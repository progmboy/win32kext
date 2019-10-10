#pragma once

#ifndef IfFailedReturn
#define IfFailedReturn(EXPR) do { hr = (EXPR); if (FAILED(hr)) { return hr; }} while(FALSE, FALSE)
#endif // IfFailedReturn

#ifdef __cplusplus
extern "C" {
#endif


#define INIT_API()                             \
    HRESULT Status;                            \
    if ((Status = ExtQuery(Client)) != S_OK) return Status;

#define EXT_RELEASE(Unk) \
    ((Unk) != NULL ? ((Unk)->Release(), (Unk) = NULL) : NULL)

#define EXIT_API     ExtRelease


	// Global variables initialized by query.
	extern PDEBUG_CLIENT4        g_ExtClient;
	extern PDEBUG_CONTROL4       g_ExtControl;
	extern PDEBUG_SYMBOLS2       g_ExtSymbols;

	extern BOOL  Connected;
	extern ULONG TargetMachine;

	HRESULT
		ExtQuery(PDEBUG_CLIENT4 Client);

	void
		ExtRelease(void);

	void __cdecl
		ExtOutDml(PCSTR Format, ...);

#ifdef __cplusplus
}
#endif