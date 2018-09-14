#pragma once

#include <windows.h>

#define STYPE_BITMAP    0L
#define STYPE_DEVICE    1L
#define STYPE_JOURNAL   2L
#define STYPE_DEVBITMAP 3L

#define BMF_1BPP       1L
#define BMF_4BPP       2L
#define BMF_8BPP       3L
#define BMF_16BPP      4L
#define BMF_24BPP      5L
#define BMF_32BPP      6L
#define BMF_4RLE       7L
#define BMF_8RLE       8L

#define PDEV_SURFACE            0x80000000  // specifies the surface is for a pdev
#define ABORT_SURFACE           0x40000000  // Abort operations on the surface
#define DYNAMIC_MODE_PALETTE    0x20000000  // The surface is a Device Dependent
// Bitmap whose palette was added
// by GreDynamicModeChange
#define UNREADABLE_SURFACE      0x10000000  // Reads not allowed from this surface
#define PALETTE_SELECT_SET      0x08000000  // We wrote palette in at select time.
#define DELETEABLE_PUBLIC_SURF  0x04000000  // deleteable even though user made public
#define BANDING_SURFACE         0x02000000  // used for banding
#define LAZY_DELETE_SURFACE     0x01000000  // DeleteObject has been called
#define DDB_SURFACE             0x00800000  // Non-monochrome Device Dependent
// Bitmap surface

#define SURF_FLAGS              0xff800000  // Logical OR of all above flags



typedef struct _SURFOBJ
{
	HANDLE dhsurf;
	HANDLE hsurf;
	HANDLE dhpdev;
	HANDLE hdev;
	SIZEL sizlBitmap;
	ULONG cjBits;
	int field_2C;
	PVOID pvBits;
	PVOID pvScan0;
	LONG lDelta;
	ULONG iUniq;
	ULONG iBitmapFormat;
	WORD iType;
	WORD fjBitmap;
}SURFOBJ, *PSURFOBJ;

typedef struct _SURFACE
{
	HANDLE Handle;
	HANDLE field_8;
	HANDLE field_10;
	SURFOBJ so;
	__int64 field_68;
	int field_70;
	int field_74;
	int field_78;
	int field_7C;
	PVOID pPal;
	__int64 field_88;
	__int64 field_90;
	__int64 field_98;
	__int64 field_A0;
	int field_A8;
	int field_AC;
	__int64 field_B0;
	int ProcessId;
	int field_BC;
	__int64 field_C0;
	__int64 field_C8;
	int field_D0;
	int field_D4;
	__int64 field_D8;
	int field_E0;
	int field_E4;
	__int64 field_E8;
	LIST_ENTRY list1;
	__int64 field_100;
	__int64 field_108;
	__int64 field_110;
	__int64 field_118;
	__int64 field_120;
	__int64 field_128;
	__int64 field_130;
	__int64 field_138;
	__int64 field_140;
	int field_148;
	int field_14C;
	__int64 field_150;
	__int64 field_158;
	__int64 field_160;
	__int64 field_168;
	__int64 field_170;
	__int64 field_178;
	__int64 field_180;
	__int64 field_188;
	__int64 field_190;
	__int64 field_198;
	__int64 field_1A0;
	__int64 field_1A8;
	__int64 field_1B0;
	__int64 field_1B8;
	__int64 field_1C0;
	LIST_ENTRY list2;
	__int64 field_1D8;
	__int64 field_1E0;
	__int64 field_1E8;
	__int64 field_1F0;
	__int64 field_1F8;
	__int64 field_200;
	__int64 field_208;
	__int64 field_210;
	__int64 field_218;
	int field_220;
	int field_224;
	int field_228;
	int field_22C;
	__int64 field_230;
	__int64 field_238;
	__int64 field_240;
	__int64 field_248;
	int field_250;
	int field_254;
	__int64 field_258;
	__int64 field_260;
	__int64 field_268;
	__int64 field_270;
}SURFACE, *PSURFACE;

