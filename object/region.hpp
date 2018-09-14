#pragma once

#include <windows.h>

#define NEG_INFINITY   0x80000000
#define POS_INFINITY   0x7fffffff


typedef struct _BASEOBJECT
{
	HANDLE     hHmgr;
	PVOID      pEntry;
	LONG       cExclusiveLock;
	PVOID	   Tid;
} BASEOBJECT, *POBJ;


typedef struct _SCAN
{
	ULONG       cWalls;
	LONG        yTop;
	LONG        yBottom;
	LONG		ai_x[1];
}SCAN, *PSCAN;

typedef struct _REGION
{
	HANDLE hHmgr;
	ULONG_PTR cExclusiveLock;
	PVOID Tid;
	ULONG sizeObj;
	ULONG UniqueId;
	ULONG cRefs;
	ULONG field_24;
	PSCAN pscnTail;
	LIST_ENTRY List;
	ULONG field_40;
	ULONG field_44;
	ULONG field_48;
	ULONG field_4C;
	ULONG sizeRgn;
	ULONG cScans;
	RECTL rcl;
	SCAN scan;
}REGION;
