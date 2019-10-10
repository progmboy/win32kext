
#include "stdafx.h"
#include "dbgext.hpp"
#include "handle.hpp"

#include "utils.hpp"
#include "SimpleOpt.h"


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


#define HMINDEXBITS             0x0000FFFF      // bits where index is stored
#define HMUNIQSHIFT             16              // bits to shift uniqueness
#define HMUNIQBITS              0xFFFF          // valid uniqueness bits
#define HMIndexFromHandle(h)    (((DWORD)h) & HMINDEXBITS)
#define HMUniqFromHandle(h)     ((WORD)((((DWORD)h) >> HMUNIQSHIFT) & HMUNIQBITS))

#define OCF_THREADOWNED      0x0001
#define OCF_PROCESSOWNED     0x0002
#define OCF_MARKTHREAD       0x0004
#define OCF_USEQUOTA         0x0008

const LPCSTR gTypeUnknonw = "Unknown";

LPCSTR
GetUserTypeDesc(
	IN UCHAR Type
)
{
	LPCSTR lpTypeDesc = gTypeUnknonw;
	for (int i = 0; i < _countof(gUserTypeDesc); i++) {
		if (gUserTypeDesc[i].Type == Type) {
			lpTypeDesc = gUserTypeDesc[i].lpszTypeDesc;
			break;
		}
	}
	return lpTypeDesc;
}

BOOL
UserObjectGetEntry(
	IN UINT32 Index,
	OUT PHANDLEENTRY pphe,
	OUT PKERNEL_HANDLEENTRY pKrlEntry,
	OPTIONAL OUT PUSER_TYPE_DESC pCreateInfo
);

EXTERN_C
HRESULT
CALLBACK
uh(PDEBUG_CLIENT4 Client, PCSTR args)
{
	ULONG64 hUser;
	hUser = GetExpression(args);

	ULONG Index = HMIndexFromHandle(hUser);
	WORD wUniq = HMUniqFromHandle(hUser);

	HANDLEENTRY HandleEntry;
	KERNEL_HANDLEENTRY KernelHandleEntry;
	USER_TYPE_DESC CreateInfo;

	INIT_API();

	do 
	{
		BOOL bRet = UserObjectGetEntry(Index, &HandleEntry, &KernelHandleEntry, &CreateInfo);
		if (!bRet){
			break;
		}

		if (KernelHandleEntry.pOwner) {

			ULONG_PTR Handle;
			LPCSTR lpszOwnerType = "Unknown";
			ULONG_PTR pObject = NULL;

			//
			// read the handle from object
			// the user object always start with "HEAD"
			//

			if (!ReadPointer((ULONG64)KernelHandleEntry.pObject, &Handle)) {
				dprintf("can not handle at 0x%p\n", KernelHandleEntry.pObject);
				break;
			}

			if (CreateInfo.CreateFlag & OCF_PROCESSOWNED) {

				//
				// the owner is PROCESSINFO
				// W32PROCESS
				//

				lpszOwnerType = "Process";

				if (!ReadPointer((ULONG64)KernelHandleEntry.pOwner, &pObject)) {
					dprintf("can not handle at 0x%p\n", KernelHandleEntry.pOwner);
					break;
				}

			}else if (CreateInfo.CreateFlag & OCF_THREADOWNED) {

				//
				// The Owner is THREADINFO
				// W32THREAD
				//

				ULONG_PTR pWin32Thread;
				lpszOwnerType = "Thread";

				if (!ReadPointer((ULONG64)KernelHandleEntry.pOwner, &pWin32Thread)) {
					dprintf("can not handle at 0x%p\n", KernelHandleEntry.pOwner);
					break;
				}

				//
				// So here is _ETHREAD 
				//

				if (GetFieldValue(pWin32Thread, "nt!_KTHREAD", "Process", pObject)) {
					dprintf("can not process at thread 0x%p\n", pWin32Thread);
					break;
				}
			}else{

			}

			ExtOutDml(
				"Object Handle     0x%llx\n"
				"Object Type       %s(%d)\n"
 				"Create Flag       0x%x\n"
				"Object            0x%p\n"
				"pOwner            0x%p\n"
				"process           <link cmd=\"!process 0x%p\">0x%p</link>\n",
				hUser,
				GetUserTypeDesc(HandleEntry.Type), (ULONG)HandleEntry.Type,
				CreateInfo.CreateFlag,
				KernelHandleEntry.pObject,
				KernelHandleEntry.pOwner,
				pObject, pObject
				);
		}
	} while (FALSE);

	EXIT_API();

	return S_OK;
}

LPCSTR
GetGdiTypeDesc(
	IN UCHAR Type
)
{
	LPCSTR lpTypeDesc = gTypeUnknonw;
	for (int i = 0; i < _countof(gGdiTypeDesc); i++) {
		if (gGdiTypeDesc[i].Type == Type) {
			lpTypeDesc = gGdiTypeDesc[i].lpszTypeDesc;
			break;
		}
	}
	return lpTypeDesc;
}

BOOL
DirectoryGetEntry(
	IN UINT32 Index,
	OUT PVOID* ppEntry,
	OUT PVOID* pLookupEntry
)
{
	ULONG uReturn;
	BOOL bRet;

	HANDLEMGR HandleMgr;
	WDBG_PTR kHandleMgr = GetExpression("poi(win32kbase!gpHandleManager)");

	if (ppEntry) {
		*ppEntry = NULL;
	}

	if (pLookupEntry) {
		*pLookupEntry = NULL;
	}

	//
	// get entire handle mgr
	//

	bRet = ReadMemory(kHandleMgr, &HandleMgr, sizeof(HandleMgr), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpHandleManager at %p\n", kHandleMgr);
		return FALSE;
	}

	if (HandleMgr.TotalGdiHandleCounts <= 0x10000){
		Index = (USHORT)Index;
	}

	//
	// read handle directory
	//

	HANDLEENTRYDIR HandleDir;
	bRet = ReadMemory((WDBG_PTR)(ULONG_PTR)(HandleMgr.pHandleEntryDir), &HandleDir, sizeof(HandleDir), &uReturn);
	if (!bRet) {
		dprintf("can not read handle directory at %p\n", HandleMgr.pHandleEntryDir);
		return FALSE;
	}


	//
	// GdiHandleEntryDirectory::_RetrieveTableAndTableEntryIndex
	// calculate the directory index and handle index 
	//

	ULONG MaxDirGdiHandleCount = HandleDir.MaxDirGdiHandleCount;
	if (Index > (MaxDirGdiHandleCount + (UINT32)((HandleDir.DirCounts - 1) << 16))) {
		return FALSE;
	}

	UINT32 EntryIndex = Index;
	UINT32 DirIndex = 0;
	if (Index >= MaxDirGdiHandleCount) {
		DirIndex = ((Index - MaxDirGdiHandleCount) >> 16) + 1;
	}

	//
	// Read EntryTable
	//

	PVOID pEntryTableAddr = (PVOID)HandleDir.pEntryTable[DirIndex];
	HANDLEENTRYTABLE HandleEntryTable;
	bRet = ReadMemory((WDBG_PTR)pEntryTableAddr, &HandleEntryTable, sizeof(HandleEntryTable), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpHandleManager at %p\n", kHandleMgr);
		return FALSE;
	}

	//
	// Calculate Index of EntryTable
	// Here dirIndex is less than ushort_max
	//
	// the meaning of the EntryIndex:
	//
	// +--------+--------+
	// | lookup | index  |
	// +--------+--------+
	// 16       8        0
	//

	if (DirIndex) {
		EntryIndex = Index - MaxDirGdiHandleCount - ((DirIndex - 1) << 16);
	}

	//
	// Read lookup table
	//

	ENTRYDATALOOKUPTABLE EntryLookupTable;
	bRet = ReadMemory((WDBG_PTR)HandleEntryTable.pEntryDataLookupTable, &EntryLookupTable, sizeof(EntryLookupTable), &uReturn);
	if (!bRet) {
		dprintf("can not read pEntryDataLookupTable at %p\n", HandleEntryTable.pEntryDataLookupTable);
		return FALSE;
	}

	//
	// Read lookup table entry
	//

	PLOOKUP_ENTRY pLookEntris;
	bRet = ReadMemory((WDBG_PTR)& EntryLookupTable.ppLookupEntries[EntryIndex >> 8],
		&pLookEntris, sizeof(pLookEntris), &uReturn);
	if (!bRet) {
		dprintf("can not read lookup Table Ptr at %p\n", EntryLookupTable.ppLookupEntries);
		return FALSE;
	}

	if (!pLookEntris){
		dprintf("lookup entry Table is null\n");
		return FALSE;
	}

	if (pLookupEntry) {
		*pLookupEntry = &pLookEntris[(UCHAR)EntryIndex];
	}

	//
	// read Handle Entry
	//

	if (ppEntry) {
		*ppEntry = &HandleEntryTable.pEntries[EntryIndex];
	}

	return TRUE;

}

EXTERN_C
HRESULT 
CALLBACK
gh(PDEBUG_CLIENT4 Client, PCSTR args)
{
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

	/*
		 +--------+--------+--------+--------+
		 |   dir  |  type  | lookup |  index |
		 +--------+--------+--------+--------+
		32       24       16        8       0
	*/

	Handle = (USHORT)Handle | ((Handle >> 8) & 0xFF0000);
	
	PVOID pEntry = NULL;
	PVOID pLookupEntry = NULL;
	if (DirectoryGetEntry(Handle, &pEntry, &pLookupEntry)) {

		//
		// Read the handle entry
		//

		ENTRY entry;
		LOOKUP_ENTRY LookupEntry;

		bRet = ReadMemory((WDBG_PTR)pLookupEntry, &LookupEntry, sizeof(LookupEntry), &uReturn);
		if (!bRet) {
			dprintf("can not read handle entry at %p\n", pEntry);
			return S_OK;
		}

		bRet = ReadMemory((WDBG_PTR)pEntry, &entry, sizeof(entry), &uReturn);
		if (!bRet) {
			dprintf("can not read handle entry at %p\n", pEntry);
			return S_OK;
		}

		if (entry.DirOrType != (USHORT)(hGdi >> 16)) {
			dprintf("!Handle Type mismatch please check it\n");
		}

		dprintf(
			"Object Type         %s(%d)\n"
			"Handle              0x%llx\n"
			"Object              0x%p\n"
			"entry               0x%p\n"
			"processx            0x%x\n",
			GetGdiTypeDesc(Type), Type,
			hGdi,
			LookupEntry.pObject,
			pEntry, entry.ProcessIdOrSome
			);
	}else{
		dprintf("invalid handle value\n");
	}
	return S_OK;
}

ULONG
GetTotalHandleCounts()
{
	BOOL bRet;
	ULONG uReturn;
	HANDLEMGR HandleMgr;
	WDBG_PTR kHandleMgr = GetExpression("poi(win32kbase!gpHandleManager)");


	//
	// get entire handle mgr
	//

	bRet = ReadMemory(kHandleMgr, &HandleMgr, sizeof(HandleMgr), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpHandleManager at %p\n", kHandleMgr);
		return 0;
	}

	return HandleMgr.TotalGdiHandleCounts;
}

typedef struct _FILTER_CONTEXT
{
	DWORD Flags;
	PVOID Process;
	int Type;
}FILTER_CONTEXT, * PFILTER_CONTEXT;

enum {
	OPT_HELP,
	OPT_FL_PROC = 1 << 0,
	OPT_FL_TYPE = 1 << 1
};
CSimpleOptA::SOption g_Options[] =
{
	{ OPT_HELP, "-h", SO_NONE },
	{ OPT_HELP, "-?", SO_NONE },
	{ OPT_HELP, "--help", SO_NONE },
	{ OPT_FL_PROC, "-p", SO_REQ_SEP },
	{ OPT_FL_TYPE, "-t", SO_REQ_SEP },
	SO_END_OF_OPTIONS
};

VOID
ShowDumpGdiHandleHelp()
{
	dprintf("Usage: !dgh [args]\n");
	dprintf("\n");
	dprintf("args list:\n");
	dprintf("-p [process] filter object by process\n");
	dprintf("-t [type id] filter object by type id\n");
	dprintf("   valid type:\n");

	LPCSTR lpTypeDesc = gTypeUnknonw;
	for (int i = 0; i < _countof(gGdiTypeDesc); i++) {
		dprintf("   id:%d - %s\n", gGdiTypeDesc[i].Type, gGdiTypeDesc[i].lpszTypeDesc);
	}

	dprintf("example:\n");
	dprintf("!dgh\n");
	dprintf("    will dump gdi object in system\n");
	dprintf("!dgh -p 0xffffffff13450080\n");
	dprintf("    will dump gdi object create by process 0xffffffff13450080\n");
	dprintf("!dgh -t 5\n");
	dprintf("    will dump all bitmap object\n");
	dprintf("!dgh -t 1 -p 0xffffffff13450080\n");
	dprintf("    will dump all bitmap object create by process 0xffffffff13450080\n");
}


BOOL
DumpGdiHandle(
	IN PFILTER_CONTEXT Context
)
{
	/*
	 +--------+--------+--------+--------+
	 |   dir  |  type  | lookup |  index |
	 +--------+--------+--------+--------+
	32       24       16        8       0
	*/

	UINT32 TotalHandleCounts;
	UINT32 HandleV;

	//
	// Check type
	//

	if (Context->Flags & OPT_FL_TYPE) {
		if (!GetGdiTypeDesc(Context->Type)) {
			dprintf("Invalid Gdi object type\n");
			return FALSE;
		}
	}

	TotalHandleCounts = GetTotalHandleCounts();

	if (!TotalHandleCounts) {
		dprintf("Have none gdi handle\n");
		return S_OK;
	}

	for (HandleV = 1; HandleV < TotalHandleCounts; HandleV++)
	{

		//
		// Get handle entry
		//

		PVOID pEntry = NULL;
		PVOID pLookupEntry = NULL;
		if (DirectoryGetEntry(HandleV, &pEntry, &pLookupEntry)) {

			//
			// Read the handle entry
			//

			BOOL bRet;
			ENTRY entry;
			LOOKUP_ENTRY LookupEntry;
			ULONG uReturn;

			bRet = ReadMemory((WDBG_PTR)pLookupEntry, &LookupEntry, sizeof(LookupEntry), &uReturn);
			if (!bRet) {
				dprintf("can not read handle entry at %p\n", pEntry);
				continue;
			}

			bRet = ReadMemory((WDBG_PTR)pEntry, &entry, sizeof(entry), &uReturn);
			if (!bRet) {
				dprintf("can not read handle entry at %p\n", pEntry);
				continue;
			}


			//
			// Is object exist? 
			//

			if (LookupEntry.pObject) {

				//
				// combine handle
				//

				ULONG Handle;

				Handle = (ULONG)entry.DirOrType << 16 | (USHORT)HandleV;

				if ((Context->Flags & OPT_FL_TYPE && Context->Type != entry.Type)){
					continue;
				}

				dprintf("Handle:0x%08x Object=0x%p Type=%s(%d) entry=0x%p processx=0x%x\n",
					(ULONG64)Handle,
					LookupEntry.pObject,
					GetGdiTypeDesc(entry.Type), entry.Type,
					pEntry, entry.ProcessIdOrSome
				);
			}
		}

		if (CheckControlC() == TRUE) {
			dprintf("user control-c break\n");
			break;
		}
	}

	return TRUE;
}

EXTERN_C
HRESULT
CALLBACK
dgh(PDEBUG_CLIENT4 Client, PCSTR args)
{
	CStringA strCmds;

	int argc;
	PSTR* argv;
	FILTER_CONTEXT FilterContext = { 0 };

	strCmds.Format("!dgh %s", args);
	argv = CommandLineToArgvA(strCmds, &argc);
	CSimpleOptA Args(argc, argv, g_Options);

	while (Args.Next()) {
		if (Args.LastError() == SO_SUCCESS) {
			switch (Args.OptionId())
			{
			case OPT_FL_PROC:
				FilterContext.Flags |= OPT_FL_PROC;
				FilterContext.Process = (PVOID)strtoull(Args.OptionArg(), NULL, 16);
				dprintf("!!!!!filter by process not implement!!!!!!!!\n");
				break;
			case OPT_FL_TYPE:
				FilterContext.Flags |= OPT_FL_TYPE;
				FilterContext.Type = strtol(Args.OptionArg(), NULL, 10);
				break;

			case OPT_HELP:
				ShowDumpGdiHandleHelp();
				return S_OK;
			default:
				break;
			}
		}
	}

	INIT_API();
	DumpGdiHandle(&FilterContext);
	EXIT_API();

	return S_OK;
}


ULONG
GetUserObjCounts()
{
	ULONG uReturn;
	BOOL bRet;

	WDBG_PTR kgSharedInfo = GetExpression("win32kbase!gSharedInfo");
	SHAREDINFO SharedInfo;
	bRet = ReadMemory(kgSharedInfo, &SharedInfo, sizeof(SharedInfo), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gSharedInfo at %p\n", kgSharedInfo);
		return 0;
	}

	//
	// read cHandleEntries from gpsi
	//

	ULONG_PTR cbHandleEntries = 0;
	bRet = ReadMemory((WDBG_PTR)SharedInfo.psi + sizeof(ULONG_PTR), &cbHandleEntries,
		sizeof(cbHandleEntries), &uReturn);
	if (!bRet) {
		dprintf("can not read gSharedInfo!cbHandleEntries at %p\n", (WDBG_PTR)SharedInfo.psi + sizeof(ULONG_PTR));
		return 0;
	}

	return (ULONG)cbHandleEntries;
}

BOOL
UserObjectGetEntry(
	IN UINT32 Index,
	OUT PHANDLEENTRY pphe,
	OUT PKERNEL_HANDLEENTRY pKrlEntry,
	OPTIONAL OUT PUSER_TYPE_DESC pCreateInfo
)
{
	ULONG uReturn;
	BOOL bRet;

	if (!pphe || !pKrlEntry){
		return FALSE;
	}

	WDBG_PTR kpKernelHandleTable = GetExpression("win32kbase!gpKernelHandleTable");
	if (!kpKernelHandleTable) {
		dprintf("can not get symbol win32kbase!gpKernelHandleTable\n");
		return FALSE;
	}
	WDBG_PTR pKernelHandleTable;
	bRet = ReadMemory(kpKernelHandleTable, &pKernelHandleTable, sizeof(pKernelHandleTable), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpKernelHandleTable at %p\n", kpKernelHandleTable);
		return FALSE;
	}


	WDBG_PTR kgSharedInfo = GetExpression("win32kbase!gSharedInfo");
	SHAREDINFO SharedInfo;
	bRet = ReadMemory(kgSharedInfo, &SharedInfo, sizeof(SharedInfo), &uReturn);
	if (!bRet) {
		dprintf("can not read win32kbase!gpKernelHandleTable at %p\n", kpKernelHandleTable);
		return FALSE;
	}

	//
	// read cHandleEntries from gpsi
	//

	ULONG_PTR cbHandleEntries = 0;
	bRet = ReadMemory((WDBG_PTR)SharedInfo.psi + sizeof(ULONG_PTR), &cbHandleEntries,
		sizeof(cbHandleEntries), &uReturn);
	if (!bRet) {
		dprintf("can not read gSharedInfo!cbHandleEntries at %p\n", (WDBG_PTR)SharedInfo.psi + sizeof(ULONG_PTR));
		return FALSE;
	}

	//
	// check the handle
	//

	if (Index > cbHandleEntries) {
		dprintf("Invalid handle\n");
		return FALSE;
	}

	if (SharedInfo.aheSize != sizeof(HANDLEENTRY)) {
		dprintf("SharedInfo.aheSize != sizeof(HANDLEENTRY) please check\n");
	}


	//
	// read handle entry
	//

	bRet = ReadMemory((WDBG_PTR)SharedInfo.aheList + (ULONG_PTR)SharedInfo.aheSize * Index, pphe,
		sizeof(*pphe), &uReturn);
	if (!bRet) {
		dprintf("can not read phe at %p\n", (WDBG_PTR)SharedInfo.aheList + (ULONG_PTR)SharedInfo.aheSize * Index);
		return FALSE;
	}
	
	//
	// Check type
	//
	
	if (pphe->Type < 1){
		return FALSE;
	}

	//
	// get the real object
	//

	bRet = ReadMemory((WDBG_PTR)pKernelHandleTable + sizeof(KERNEL_HANDLEENTRY) * Index, pKrlEntry,
		sizeof(*pKrlEntry), &uReturn);
	if (!bRet) {
		dprintf("can not read kernel phe at %p\n", (WDBG_PTR)pKernelHandleTable + sizeof(KERNEL_HANDLEENTRY) * Index);
		return FALSE;
	}

	//
	// object createflag
	//

	if (pCreateInfo){
		WDBG_PTR kgahti = GetExpression("win32kbase!gahti");
		bRet = ReadMemory((WDBG_PTR)kgahti + sizeof(USER_TYPE_DESC) * pphe->Type, pCreateInfo,
			sizeof(*pCreateInfo), &uReturn);
		if (!bRet) {
			dprintf("can not read create info at %p\n", (WDBG_PTR)kgahti + sizeof(USER_TYPE_DESC) * pphe->Type);
			return FALSE;
		}
	}


	return TRUE;
}

VOID
ShowDumpUserHandleHelp()
{
	dprintf( "Usage: !duh [args]\n");
	dprintf("\n");
	dprintf("args list:\n");
	dprintf("-p [process] filter object by process\n");
	dprintf("-t [type id] filter object by type id\n");
	dprintf("   valid type:\n");

	LPCSTR lpTypeDesc = gTypeUnknonw;
	for (int i = 0; i < _countof(gUserTypeDesc); i++) {
		dprintf("   id:%d - %s\n", gUserTypeDesc[i].Type, gUserTypeDesc[i].lpszTypeDesc);
	}

	dprintf("example:\n");
	dprintf("!duh\n");
	dprintf("    will dump user object in system\n");
	dprintf("!duh -p 0xffffffff13450080\n");
	dprintf("    will dump user object create by process 0xffffffff13450080\n");
	dprintf("!duh -t 1\n");
	dprintf("    will dump all window object\n");
	dprintf("!duh -t 1 -p 0xffffffff13450080\n");
	dprintf("    will dump all window object create by process 0xffffffff13450080\n");
}

BOOL
DumpUserHandles(
	IN PFILTER_CONTEXT Context
	)
{
	UINT32 TotalHandleCounts;
	UINT32 HandleV;

	//
	// Check type
	//

	if (Context->Flags & OPT_FL_TYPE) {
		if (!GetUserTypeDesc(Context->Type)) {
			dprintf("Invalid user object type\n");
			return FALSE;
		}
	}

	g_ExtControl->Execute(DEBUG_OUTCTL_IGNORE, ".reload nt", DEBUG_EXECUTE_NOT_LOGGED);
	g_ExtControl->Execute(DEBUG_OUTCTL_IGNORE, ".reload win32kbase.sys", DEBUG_EXECUTE_NOT_LOGGED);
	g_ExtControl->Execute(DEBUG_OUTCTL_IGNORE, ".reload win32kfull.sys", DEBUG_EXECUTE_NOT_LOGGED);

	TotalHandleCounts = GetUserObjCounts();
	if (!TotalHandleCounts) {
		dprintf("Have none user handle\n");
		return TRUE;
	}

	dprintf("Total 0x%x handles\n", TotalHandleCounts);

	for (HandleV = 1; HandleV < TotalHandleCounts; HandleV++)
	{

		HANDLEENTRY HandleEntry;
		KERNEL_HANDLEENTRY KernelHandleEntry;
		USER_TYPE_DESC CreateInfo;
		if (UserObjectGetEntry(HandleV, &HandleEntry, &KernelHandleEntry, &CreateInfo)) {

			//
			// owner == NULL mean this handle has been destroy
			//

			if (KernelHandleEntry.pOwner) {

				ULONG_PTR Handle;
				LPCSTR lpszOwnerType = "Unknown";
				ULONG_PTR pObject = NULL;

				//
				// read the handle from object
				// the user object always start with "HEAD"
				//

				if (!ReadPointer((ULONG64)KernelHandleEntry.pObject, &Handle)) {
					dprintf("can not handle at 0x%p\n", KernelHandleEntry.pObject);
					continue;
				}

				if (CreateInfo.CreateFlag & OCF_PROCESSOWNED) {

					//
					// the owner is PROCESSINFO
					// W32PROCESS
					//

					lpszOwnerType = "Process";

					if (!ReadPointer((ULONG64)KernelHandleEntry.pOwner, &pObject)) {
						dprintf("can not handle at 0x%p\n", KernelHandleEntry.pOwner);
						continue;
					}

				}else if (CreateInfo.CreateFlag & OCF_THREADOWNED) {

					//
					// The Owner is THREADINFO
					// W32THREAD
					//

					ULONG_PTR pWin32Thread;
					lpszOwnerType = "Thread";

					if (!ReadPointer((ULONG64)KernelHandleEntry.pOwner, &pWin32Thread)) {
						dprintf("can not handle at 0x%p\n", KernelHandleEntry.pOwner);
						continue;
					}

					//
					// So here is _ETHREAD 
					//

					if (GetFieldValue(pWin32Thread, "nt!_KTHREAD", "Process", pObject)) {
						dprintf("can not process at thread 0x%p\n", pWin32Thread);
						continue;
					}


				}else {

				}

				//dprintf("handle:0x%08x object=0x%p type=(%02d)%-14s owner=0x%p process=0x%p createflag=0x%04x\n",
				//	(ULONG64)Handle,
				//	KernelHandleEntry.pObject,
				//	HandleEntry.Type, GetUserTypeDesc(HandleEntry.Type),
				//	KernelHandleEntry.pOwner, pObject, CreateInfo.CreateFlag
				//);

				if ((Context->Flags & OPT_FL_PROC && Context->Process != (PVOID)pObject) ||
					(Context->Flags & OPT_FL_TYPE && Context->Type != (int)HandleEntry.Type)){
					continue;
				}

				ExtOutDml("handle=<link cmd=\"!uh 0x%08x\">0x%08x</link> object=0x%p " \
					"process=<link cmd=\"!process 0x%p\">0x%p</link> type=(%02d)%-14s\n",
					(ULONG64)Handle, (ULONG64)Handle,
					KernelHandleEntry.pObject,
					pObject, pObject,
					HandleEntry.Type, GetUserTypeDesc(HandleEntry.Type)
				);

			}
		}

		if (CheckControlC() == TRUE) {
			dprintf("user control-c break\n");
			break;
		}
	}

	return TRUE;
}


EXTERN_C
HRESULT
CALLBACK
duh(PDEBUG_CLIENT4 Client, PCSTR args)
{
	
	CStringA strCmds;

	int argc;
	PSTR* argv;
	FILTER_CONTEXT FilterContext = {0};

	strCmds.Format("!duh %s", args);
	argv = CommandLineToArgvA(strCmds, &argc);
	CSimpleOptA Args(argc, argv, g_Options);

	while (Args.Next()) {
		if (Args.LastError() == SO_SUCCESS) {
			switch (Args.OptionId())
			{
			case OPT_FL_PROC:
				FilterContext.Flags |= OPT_FL_PROC;
				FilterContext.Process = (PVOID)strtoull(Args.OptionArg(), NULL, 16);
				break;
			case OPT_FL_TYPE:
				FilterContext.Flags |= OPT_FL_TYPE;
				FilterContext.Type = strtol(Args.OptionArg(), NULL, 10);
				break;

			case OPT_HELP:
				ShowDumpUserHandleHelp();
				return S_OK;
			default:
				break;
			}
		}
	}

	INIT_API();
	DumpUserHandles(&FilterContext);
	EXIT_API();

	return S_OK;
}

