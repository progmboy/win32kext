#pragma once

#include <windows.h>

struct _PALETTE;
typedef struct _PALETTE PALETTE;

typedef union _HDEVPPAL
{
	HANDLE hdev;
	PALETTE* ppal;
} HDEVPPAL;

typedef struct _PALETTE
{
	HPALETTE Handle;
	ULONG_PTR field_8;
	ULONG_PTR field_10;
	ULONG flPal;
	ULONG cEntries;
	ULONG ulTime;
	ULONG field_24;
	HDC hdcHead;
	HDEVPPAL hSelected;
	ULONG cRefhpal;
	ULONG cRefRegular;
	ULONG field_40;
	ULONG field_44;
	PVOID ptransFore;
	PVOID ptransCurrent;
	PVOID ptransOld;
	HANDLE hcmXform;
	PALETTE* ppalNext;
	PVOID apalColor;
	PVOID ppalColor;
	PVOID apalColorTable;
}PALETTE ,*PPALETTE;


