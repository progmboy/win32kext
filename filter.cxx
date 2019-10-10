
#include "stdafx.h"
#include <string>

typedef struct _SYSCALL_FILTER
{
	LPSTR *lpFuncName;
	ULONGLONG nCounts;
}SYSCALL_FILTER, *PSYSCALL_FILTER;

BOOL gFilterListInit = FALSE;
SYSCALL_FILTER gSyscallFilterList[7] = {0};

typedef enum _FILTER_LEVEL
{
  NormalAPP = 0x0,
  Rs1RestrictedAppcontainer = 0x1,
  Rs1RestrictedAppcontainerPlugin = 0x2,
  FontDrvHost = 0x3,
  Rs1RestrictedAppcontainerMiniPlugin = 0x4,
  Rs3RestrictedAppcontainer = 0x5,
  Rs3HvsiRdpClient = 0x6,
}FILTER_LEVEL;


BOOL
ReadString(
	WDBG_PTR ppstr,
	std::string &strRead
)
{
	int i = 0;

	strRead = "";
	while (1)
	{
		CHAR c;
		if(!ReadMemory((ULONG_PTR)ppstr + i, &c, 1, NULL)){
			return FALSE;
		}

		if (!c){
			break;
		}

		strRead += c;
		i++;
	}

	return TRUE;
}

EXTERN_C
HRESULT CALLBACK
fl(PDEBUG_CLIENT4 Client, PCSTR args)
{
	ULONG Offset;
	ULONG FilterSet = 0;
	
	if (!gFilterListInit){
		
		//
		// read from win32kbase
		//
		
		WDBG_PTR kgSharedInfo = GetExpression("win32kbase!gaWin32KSyscallList");

		ReadMemory(kgSharedInfo, gSyscallFilterList, sizeof(gSyscallFilterList), NULL);
		
		gFilterListInit = TRUE;
	}

	WDBG_PTR pProcess = GetExpression("$proc");
	GetFieldOffset("nt!_EPROCESS", "Win32KFilterSet", &Offset);
	ReadMemory(pProcess + Offset, &FilterSet, sizeof(FilterSet), NULL);
	dprintf("current proc filter set: %d\n", FilterSet);
	if (FilterSet < 0 && FilterSet > 6) {
		dprintf("level in [0-6]\n");
		return S_OK;
	}
	//iLevel = FilterSet;


	if (!gSyscallFilterList[FilterSet].nCounts || !gSyscallFilterList[FilterSet].lpFuncName){
		dprintf("no filter\n");
		return S_OK;
	}


	dprintf("allow list:\n");
	for (int i = 0; i < gSyscallFilterList[FilterSet].nCounts; i++) {
		std::string strRead;

		//
		// read the string pointer
		//
		
		ULONG_PTR pFuncNameAddr = (ULONG_PTR)gSyscallFilterList[FilterSet].lpFuncName + i * sizeof(ULONG_PTR);
		ULONG_PTR pFuncName;
		if (!ReadMemory(pFuncNameAddr, (PVOID)&pFuncName, sizeof(pFuncName), NULL)) {
			break;
		}

		//
		// read the function name
		//
		
		if (!ReadString((WDBG_PTR)pFuncName, strRead)){
			break;
		}
		dprintf("\t\t%s\n", strRead.c_str());
	}

	return S_OK;
}