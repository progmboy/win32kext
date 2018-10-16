
#include "stdafx.h"
#include "handle.hpp"

WINDBG_EXTENSION_APIS   ExtensionApis = { 0 };


EXTERN_C
WIN32KEXT_API
HRESULT
CALLBACK
DebugExtensionInitialize(
	OUT PULONG Version, 
	OUT PULONG Flags)
{
	IDebugClient *DebugClient;
	PDEBUG_CONTROL4 DebugControl;
	HRESULT Hr;

	*Version = DEBUG_EXTENSION_VERSION(1, 0);
	*Flags = 0;
	Hr = S_OK;
	
	//
	// Create a new debugclient
	//
	
	if ((Hr = DebugCreate(__uuidof(IDebugClient), (void **)&DebugClient)) != S_OK){
		return Hr;
	}

	if ((Hr = DebugClient->QueryInterface(__uuidof(IDebugControl4), 
		(void **)&DebugControl)) == S_OK){

		//
		// Get the windbg-style extension APIS
		//

		ExtensionApis.nSize = sizeof(ExtensionApis);
		Hr = DebugControl->GetWindbgExtensionApis64(&ExtensionApis);

		//
		// check is in kernel mode
		//

		ULONG dbgClass, dbgQualifier;
		if (SUCCEEDED(DebugControl->GetDebuggeeType(&dbgClass, &dbgQualifier))){
			if (dbgClass != DEBUG_CLASS_KERNEL) {
				dprintf("This extension is for kernel-mode only\n");
				Hr = S_FALSE;
			}
		}
		
		//
		// check os
		//
		
		ULONG PlatformId, Win32Major, Win32Minor, KdMajor, KdMinor;
		if (SUCCEEDED(DebugControl->GetSystemVersionValues(&PlatformId, &Win32Major, &Win32Minor,
			&KdMajor, &KdMinor))){
			DEBUGPRINT("OS Detected: %d.%d.%d.%d\n", Win32Major, Win32Minor, KdMajor, KdMinor);
		}


		DebugControl->Release();

	}
	DebugClient->Release();
	return Hr;
}

typedef struct _TYPE_DESC
{
	int Type;
	LPCSTR lpszTypeDesc;
}TYPE_DESC, *PTYPE_DESC;

TYPE_DESC gGdiTypeDesc[] = 
{ 
  { 0x01, "DC" },
  { 0x02, "ColorTransform" },
  { 0x04, "Rgn" },
  { 0x05, "Bitmap" },
  { 0x07, "Path" },
  { 0x08, "Palette" },
  { 0x09, "ColorSpace" },
  { 0x0a, "Font" },
  { 0x0e, "ColorTransform" },
  { 0x0f, "Sprite" },
  { 0x10, "Brush" },
  { 0x12, "LogicSurface" },
  { 0x13, "Space" },
  { 0x15, "ServerMetafile" },
  { 0x1c, "Driver" },
  { 0x8a, "Font2" },
};

TYPE_DESC gUserTypeDesc[] =
{
	{ 0x01, "Window" },          //0x190
	{ 0x02, "Menu" },            //0xb0
	{ 0x03, "Cursor" },			 //0x98
	{ 0x04, "DeferWindowPos" },  //0x30
	{ 0x05, "WindowHook" },      //0x60
	{ 0x06, "MemoryHandle" },
	{ 0x07, "CPD" },		     //0x48
	{ 0x08, "AcceleratorTable" },
	{ 0x09, "CsDde" },           //0x40
	{ 0x0a, "Conversation" },    //0x60
	{ 0x0b, "pxs" },             //0x48
	{ 0x0c, "Monitor" },         //0x260
	{ 0x0d, "Keyboard" },        //0x78
	{ 0x0e, "KeyboardLayout" },  //0x78
	{ 0x0f, "EventHook" },       //0x60
	{ 0x10, "Timer" },           //0x88
	{ 0x11, "InputContext" },    //0x48
	{ 0x12, "HidData" },
	{ 0x14, "TouchInputInfo" },
	{ 0x15, "GestureInfo" },
	{ 0x17, "BaseWindow" }      //0x80
};

EXTERN_C
WIN32KEXT_API
void 
CALLBACK 
DebugExtensionUninitialize(void)
{
	DEBUGPRINT("DebugExtensionUninitialize\n");
	return;
}


#define HMINDEXBITS             0x0000FFFF      // bits where index is stored
#define HMUNIQSHIFT             16              // bits to shift uniqueness
#define HMUNIQBITS              0xFFFF          // valid uniqueness bits
#define HMIndexFromHandle(h)    (((DWORD)h) & HMINDEXBITS)
#define HMUniqFromHandle(h)     ((WORD)((((DWORD)h) >> HMUNIQSHIFT) & HMUNIQBITS))

EXTERN_C
HRESULT
CALLBACK
uh(PDEBUG_CLIENT4 Client, PCSTR args)
{
	ULONG uReturn;
	BOOL bRet;

	ULONG64 hUser;
	hUser = GetExpression(args);

	ULONG Index = HMIndexFromHandle(hUser);
	WORD wUniq = HMUniqFromHandle(hUser);
	WDBG_PTR kpKernelHandleTable = GetExpression("win32kbase!gpKernelHandleTable");
	if (!kpKernelHandleTable){
		dprintf("can not get symbol win32kbase!gpKernelHandleTable\n");
		return S_FALSE;
	}
	WDBG_PTR pKernelHandleTable;
	bRet = ReadMemory(kpKernelHandleTable, &pKernelHandleTable, sizeof(pKernelHandleTable), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpKernelHandleTable at %p\n", kpKernelHandleTable);
		return S_FALSE;
	}


	WDBG_PTR kgSharedInfo = GetExpression("win32kbase!gSharedInfo");
	SHAREDINFO SharedInfo;
	bRet = ReadMemory(kgSharedInfo, &SharedInfo, sizeof(SharedInfo), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpKernelHandleTable at %p\n", kpKernelHandleTable);
		return S_FALSE;
	}
	
	//
	// read cHandleEntries from gpsi
	//
	
	ULONG_PTR cbHandleEntries = 0;
	bRet = ReadMemory((WDBG_PTR)SharedInfo.psi + sizeof(ULONG_PTR), &cbHandleEntries, 
		sizeof(cbHandleEntries), &uReturn);
	if (!bRet) {
		dprintf("can not read gSharedInfo!cbHandleEntries at %p\n", (WDBG_PTR)SharedInfo.psi + sizeof(ULONG_PTR));
		return S_FALSE;
	}
	
	//
	// cheack the handle
	//
	
	if (Index > cbHandleEntries) {
		dprintf("Invalid handle\n");
		return S_FALSE;
	}

	if (SharedInfo.aheSize != sizeof(HANDLEENTRY)){
		dprintf("SharedInfo.aheSize != sizeof(HANDLEENTRY) please check\n");
	}


	//
	// read handle entry
	//
	
	HANDLEENTRY phe;
	
	bRet = ReadMemory((WDBG_PTR)SharedInfo.aheList + SharedInfo.aheSize * Index, &phe,
		sizeof(phe), &uReturn);
	if (!bRet) {
		dprintf("can not read phe at %p\n", (WDBG_PTR)SharedInfo.aheList + SharedInfo.aheSize * Index);
		return S_FALSE;
	}

	if (wUniq != -1 && wUniq != phe.wUniq){
		dprintf("uniq number do not match please check the handle the result may be wrong!\n");
	}

	
	//
	// get the real object
	//
	
	KERNEL_HANDLEENTRY KernelEntry;
	bRet = ReadMemory((WDBG_PTR)pKernelHandleTable + sizeof(KERNEL_HANDLEENTRY) * Index, &KernelEntry,
		sizeof(KernelEntry), &uReturn);
	if (!bRet) {
		dprintf("can not read kernel phe at %p\n", (WDBG_PTR)pKernelHandleTable + sizeof(KERNEL_HANDLEENTRY) * Index);
		return S_FALSE;
	}
	
	LPCSTR lpTypeDesc = "Unknown";
	for (int i = 0; i < _countof(gUserTypeDesc); i++) {
		if (gUserTypeDesc[i].Type == phe.Type) {
			lpTypeDesc = gUserTypeDesc[i].lpszTypeDesc;
			break;
		}
	}
	
	//
	// create object createflag
	//
	
	WDBG_PTR kgahti = GetExpression("win32kbase!gahti");
	USER_TYPE_DESC CreateInfo;
	bRet = ReadMemory((WDBG_PTR)kgahti + sizeof(USER_TYPE_DESC) * phe.Type, &CreateInfo,
		sizeof(CreateInfo), &uReturn);
	if (!bRet) {
		dprintf("can not read create info at %p\n", (WDBG_PTR)kgahti + sizeof(USER_TYPE_DESC) * phe.Type);
		return S_FALSE;
	}
	 

	dprintf(
		"Object Type       %s(%d)\n"
		"Create Flag       0x%x\n"
		"Object Handle     0x%llx\n"
		"Object            0x%p\n"
		"pOwner            0x%p\n",
		lpTypeDesc, phe.Type, CreateInfo.CreateFlag, hUser, KernelEntry.pObject, KernelEntry.pOwner);
	
	return S_OK;
}


EXTERN_C
HRESULT 
CALLBACK
gh(PDEBUG_CLIENT4 Client, PCSTR args)
{
	HANDLEMGR HandleMgr;
	ULONG uReturn;
	BOOL bRet;

	ULONG64 hGdi;
	hGdi = GetExpression(args) & 0xffffffff;
	
	//
	// first check the handle is valid
	//
	
	UCHAR Type = (hGdi >> 16) & 0xFF;
	ULONG Handle = (ULONG)hGdi;

	//
	// format handle. remove the type info and 
	// move directory index
	//

	Handle = (USHORT)Handle | ((Handle >> 8) & 0xFF0000);
	
	//
	// read the handle mgr
	//

	WDBG_PTR kHandleMgr = GetExpression("win32kbase!gpHandleManager");
	WDBG_PTR pgHandleMgr;

	bRet = ReadMemory(kHandleMgr, &pgHandleMgr, sizeof(pgHandleMgr), &uReturn);
	if (!bRet) {
		dprintf("can not read gpHandleManager at %p\n", kHandleMgr);
		return S_FALSE;
	}

	bRet = ReadMemory(pgHandleMgr, &HandleMgr, sizeof(HandleMgr), &uReturn);
	if (!bRet){
		dprintf("can not read win32kbase!gpHandleManager at %p\n", pgHandleMgr);
		return S_FALSE;
	}
	
	//
	// read handle directory
	//
	
	HANDLEENTRYDIR HandleDir;
	bRet = ReadMemory((WDBG_PTR)(ULONG_PTR)(HandleMgr.pHandleEntryDir), &HandleDir, sizeof(HandleDir), &uReturn);
	if (!bRet) {
		dprintf("can not read handle directory at %p\n", HandleMgr.pHandleEntryDir);
		return S_FALSE;
	}

	if (Handle > 0x10000){
		Handle = (ULONG)((USHORT)Handle);
	}
	
	if (Handle > (HandleDir.MaxGdiHandleCountUserSet + ((HandleDir.DirIndex + 0xFFFF) << 16))){
		dprintf("invalid handle value\n");
		return S_FALSE;
	}

	ULONG DirIndex = 0;
	if (Handle > HandleDir.MaxGdiHandleCountUserSet){
		DirIndex = ((Handle - HandleDir.MaxGdiHandleCountUserSet) >> 16) + 1;
		if (DirIndex == (ULONG)-1){
			dprintf("invalid handle value\n");
			return S_FALSE;
		}
	}

	PHANDLEENTRYTABLE pTable = HandleDir.pEntryTable[DirIndex];
	if (DirIndex)
		Handle = Handle - (DirIndex << 16) - HandleDir.MaxGdiHandleCountUserSet + 0x10000;

	
	//
	// read Entry table
	//
	
	HANDLEENTRYTABLE HandleEntryTable;
	bRet = ReadMemory((WDBG_PTR)(ULONG_PTR)pTable, &HandleEntryTable, sizeof(HandleEntryTable), &uReturn);
	if (!bRet) {
		dprintf("can not read handle entry Table at %p\n", pTable);
		return S_FALSE;
	}

	//
	// read lookup entry table
	//
	
	ENTRYDATALOOKUPTABLE LookupTable;
	bRet = ReadMemory((WDBG_PTR)(ULONG_PTR)HandleEntryTable.pEntryDataLookupTable, //ffff84b4`80001010
		&LookupTable, sizeof(LookupTable), &uReturn);
	if (!bRet) {
		dprintf("can not read lookup Table at %p\n", HandleEntryTable.pEntryDataLookupTable);
		return S_FALSE;
	}

	//
	// read the dest lookup 
	//
	
	if (Handle >= HandleEntryTable.CurLookupIndex){
		dprintf("invalid handle value\n");
		return S_FALSE;
	}


	WDBG_PTR pLookupEntryPtrOff = (WDBG_PTR)LookupTable.ppLookupEntries + (Handle >> 8) * sizeof(PVOID);

	PLOOKUP_ENTRY *ppLookEntry;
	bRet = ReadMemory(pLookupEntryPtrOff, &ppLookEntry, sizeof(ppLookEntry), &uReturn);
	if (!bRet) {
		dprintf("can not read lookup Table Ptr at %p\n", HandleEntryTable.pEntryDataLookupTable);
		return S_FALSE;
	}

	WDBG_PTR pLoolupEntry = (WDBG_PTR)ppLookEntry + (UCHAR)Handle * sizeof(LOOKUP_ENTRY);
	LOOKUP_ENTRY lookEntry;
	bRet = ReadMemory(pLoolupEntry, &lookEntry, sizeof(lookEntry), &uReturn);
	if (!bRet){
		dprintf("can not read lookup Table at %p\n", pLoolupEntry);
		return S_FALSE;
	}

	//
	// get entry of this handle
	//
	
	WDBG_PTR EntryOff = (WDBG_PTR)HandleEntryTable.pEntries + Handle * sizeof(ENTRY);
	ENTRY entry;
	bRet = ReadMemory(EntryOff, &entry, sizeof(entry), &uReturn);
	if (!bRet) {
		dprintf("can not read lookup Table at %p\n", EntryOff);
		return S_FALSE;
	}

	//DEBUGPRINT("type=%d, dir_or_type=%d", Type, (UCHAR)entry.DirOrType);

	if (entry.DirOrType != (USHORT)(hGdi >> 16)){
		dprintf("!Handle Type mismatch please check it\n");
	}

	LPCSTR lpTypeDesc = "Unknown";
	for (int i = 0; i < _countof(gGdiTypeDesc); i++) {
		if (gGdiTypeDesc[i].Type == Type){
			lpTypeDesc = gGdiTypeDesc[i].lpszTypeDesc;
			break;
		}
	}

	dprintf(
		"Object Type         %s(%d)\n"
		"Handle              0x%llx\n"
		"Object              0x%p\n", 
		lpTypeDesc, Type,
		hGdi, 
		lookEntry.pObject);
	
	return S_OK;
}

