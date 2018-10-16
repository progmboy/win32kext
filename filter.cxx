
#include "stdafx.h"
#include <string>

typedef struct _SYSCALL_FILTER
{
	LPSTR *lpFuncName;
	ULONGLONG nCounts;
}SYSCALL_FILTER, *PSYSCALL_FILTER;

BOOL gFilterListInit = FALSE;
SYSCALL_FILTER gSyscallFilterList[7] = {0};

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
	ULONG iLevel = strtoul(args, NULL, 10);
	if (iLevel < 0 && iLevel > 6){
		dprintf("level in [0-6]\n");
		return S_OK;
	}

	if (!gFilterListInit){
		
		//
		// read from win32kbase
		//
		
		WDBG_PTR kgSharedInfo = GetExpression("win32kbase!gaWin32KSyscallList");

		ReadMemory(kgSharedInfo, gSyscallFilterList, sizeof(gSyscallFilterList), NULL);
		
		gFilterListInit = TRUE;
	}


	if (!gSyscallFilterList[iLevel].nCounts || !gSyscallFilterList[iLevel].lpFuncName){
		dprintf("no filter\n");
		return S_OK;
	}

	dprintf("allow list:\n");
	for (int i = 0; i < gSyscallFilterList[iLevel].nCounts; i++) {
		std::string strRead;

		//
		// read the string pointer
		//
		
		ULONG_PTR pFuncNameAddr = (ULONG_PTR)gSyscallFilterList[iLevel].lpFuncName + i * sizeof(ULONG_PTR);
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