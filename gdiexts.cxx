
#include "stdafx.h"
#include "object/region.hpp"
#include "object/suface.hpp"
#include "object/palette.hpp"

VOID Gdidr(
	REGION *  prgn
);

VOID Gdicr(
	REGION * prgn
);

VOID Gdidppal(
	PALETTE * pvServer
);

VOID Gdidpsurf(
	PVOID pvServer
);

#define DbgPrint

/**************************************************************************\
*
* move(dst, src ptr)
*
\**************************************************************************/

#define move(dst, src)							\
    ReadMemory((ULONG_PTR)(src), &(dst), sizeof(dst), NULL);

/**************************************************************************\
*
* move2(dst ptr, src ptr, num bytes)
*
\**************************************************************************/
#define move2(dst, src, size)						\
    ReadMemory((ULONG_PTR) (src), &(dst), (size), NULL);


EXTERN_C
HRESULT CALLBACK
dr(PDEBUG_CLIENT4 Client, PCSTR args)
{
	/*ULONG prgn;

	if (*args != '\0')
		sscanf(args, "%lx", &prgn);
	else
	{
		dprintf("Please supply an argument \n");
		return;
	}*/

	ULONG_PTR prgn;
	prgn = GetExpression(args);
	if (!prgn){
		dprintf("Please supply an argument \n");
		return S_OK;
	}

	Gdidr((REGION *)prgn);
	return S_OK;
}


/******************************Public*Routine******************************\
* VOID Gdidr (
*     PVOID prgn
*     )
*
* Debugger extension to dump a region.
*
* History:
*  14-Feb-1992 -by- Mike Harrington [Mikehar]
* Wrote it.
\**************************************************************************/

VOID Gdidr(
	REGION *  prgn
)
{
	REGION  rgn;
	PSCAN   pscnHead;
	//ULONG   i;
	BOOL bCheck = FALSE;

	move(rgn, prgn);
	//pscnHead = (PSCAN)((PBYTE)prgn + sizeof(rgn));
	pscnHead = (PSCAN)((PBYTE)prgn + offsetof(REGION, scan));

	dprintf(
		"hHmgr     0x%p\n"
		"cExLock   0x%p\n"
		"tid       0x%p\n"
		"sizeObj   0x%lx\n"
		"sizeRgn   0x%lx\n"
		"cRefs     %ld\n"
		"cScans    %ld\n"
		"rcl       {0x%lx 0x%lx 0x%lx 0x%lx}\n"
		"pscnHead  0x%p\n"
		"pscnTail  0x%p\n",

		rgn.hHmgr,
		rgn.cExclusiveLock,
		rgn.Tid,
		rgn.sizeObj,
		rgn.sizeRgn,
		rgn.cRefs,
		rgn.cScans,
		rgn.rcl.left, rgn.rcl.top, rgn.rcl.right, rgn.rcl.bottom,
		pscnHead,
		rgn.pscnTail);

   /*
	* make the region data accessible.
	*/

	/*
	i = 0;

	{
		PSCAN   pscn = (PSCAN)(prgn + 1);
		ULONG   cscn = rgn.cScans;

		LONG    lPrevBottom = NEG_INFINITY;
		LONG    lPrevRight;

		while (cscn--)
		{
			LONG yTop;
			LONG yBottom;
			LONG cWalls;
			LONG cWalls2;
			LONG left;
			LONG right;
			ULONG iWall = 0;

			move(yTop, (PBYTE)pscn + offsetof(SCAN, yTop));
			move(yBottom, (PBYTE)pscn + offsetof(SCAN, yBottom));
			move(cWalls, (PBYTE)pscn + offsetof(SCAN, cWalls));

			if (bCheck){
				if (yTop < lPrevBottom)
				{
					dprintf("top < prev bottom, scan %ld, pscn @ 0x%lx\n",
						rgn.cScans - cscn, (BYTE *)pscn - (BYTE *)prgn);
					return;
				}

				if (yTop > yBottom)
				{
					dprintf("top > bottom, scan %ld, pscn @ 0x%lx\n",
						rgn.cScans - cscn, (BYTE *)pscn - (BYTE *)prgn);
					return;
				}
			}


			lPrevBottom = yBottom;
			lPrevRight = NEG_INFINITY;

			while ((LONG)iWall < cWalls)
			{
				move(left, (PBYTE)pscn + offsetof(SCAN, ai_x[iWall]));
				move(right, (PBYTE)pscn + offsetof(SCAN, ai_x[iWall + 1]));

				if (bCheck) {
					if ((left <= lPrevRight) || (right <= left))
					{
						dprintf("left[i] < left[i+1], pscn @ 0x%lx, iWall = 0x%lx\n",
							(BYTE *)pscn - (BYTE *)prgn, iWall);
						return;
					}
				}

				lPrevRight = right;

				dprintf("\tRectangle #%d  { 0x%lx, 0x%lx, 0x%lx, 0x%lx }\n",
					i, left, yTop, right, yBottom);

				++i;

				iWall += 2;

				if (CheckControlC())
					return;
			}

			move(cWalls2, (PBYTE)pscn + offsetof(SCAN, ai_x[iWall]));

			if (bCheck) {
				if (cWalls != cWalls2)
				{
					dprintf("cWalls != cWalls2 @ 0x%lx\n",
						(BYTE *)pscn - (BYTE *)prgn);
					return;
				}
			}

			pscn = (PSCAN)((PBYTE)pscn + (cWalls * sizeof(LONG) + sizeof(SCAN)));

			if ((ULONG_PTR)pscn >= (ULONG_PTR)rgn.pscnTail)
			{
				//dprintf("Went past end of region\n");
				return;
			}
		}
	}*/
	
		ULONG cScans = rgn.cScans;
		PSCAN pscn = pscnHead;

		if (cScans){
			dprintf("------------dump cScan------------\n");
		}

		for (ULONG i = 0;
			i < cScans;
			i++)
		{
			ULONG cWalls;
			LONG yTop;
			LONG yBottom;
			LONG left;
			LONG right;

			move(yTop, (PBYTE)pscn + offsetof(SCAN, yTop));
			move(yBottom, (PBYTE)pscn + offsetof(SCAN, yBottom));
			move(cWalls, (PBYTE)pscn + offsetof(SCAN, cWalls));

			
			if (!cWalls){
				dprintf("\tScan #%d: yTop = 0x%lx, yBottom = 0x%lx, cWalls = %ld\n",
					i, yTop, yBottom, cWalls);
			}else{
				for (ULONG j = 0;
					j < cWalls;
					j += 2)
				{
					move(left, (PBYTE)pscn + offsetof(SCAN, ai_x[j]));
					move(right, (PBYTE)pscn + offsetof(SCAN, ai_x[j + 1]));

					dprintf("\tScan #%d: { 0x%lx, 0x%lx, 0x%lx, 0x%lx }\n",
						i, left, yTop, right, yBottom);
				}
			}
			
			//pscn = pscnGet(pscn);
			pscn = (PSCAN)((PBYTE)pscn + (cWalls * sizeof(LONG) + sizeof(SCAN)));
		}
}


EXTERN_C
HRESULT CALLBACK
cr(PDEBUG_CLIENT4 Client, PCSTR args)
{
	ULONG_PTR prgn;
	prgn = GetExpression(args);
	if (!prgn) {
		dprintf("Please supply an argument \n");
		return S_OK;
	}

	Gdicr((REGION *)prgn);
	return S_OK;
}


VOID Gdicr(
	REGION * prgn
)
{
	REGION  rgn;
	PSCAN   pscnHead;
	BOOL bCheck = TRUE;

	move(rgn, prgn);
	pscnHead = (PSCAN)((PBYTE)prgn + sizeof(rgn));

	dprintf("pr = %lx, sizeof(rgn) = %lx, pscnHead = %lx\n", prgn, sizeof(rgn), pscnHead);

	dprintf(
		"hHmgr     0x%p\n"
		"cExLock   0x%p\n"
		"tid       0x%p\n"
		"sizeObj   0x%lx\n"
		"sizeRgn   0x%lx\n"
		"cRefs     %ld\n"
		"cScans    %ld\n"
		"rcl       {0x%lx 0x%lx 0x%lx 0x%lx}\n"
		"pscnHead  0x%p\n"
		"pscnTail  0x%p\n",

		rgn.hHmgr,
		rgn.cExclusiveLock,
		rgn.Tid,
		rgn.sizeObj,
		rgn.sizeRgn,
		rgn.cRefs,
		rgn.cScans,
		rgn.rcl.left, rgn.rcl.top, rgn.rcl.right, rgn.rcl.bottom,
		pscnHead,
		rgn.pscnTail);

	if ((pscnHead > rgn.pscnTail) ||
		(rgn.sizeObj < rgn.sizeRgn))
	{
		DbgPrint("Error in region\n");
		return;
	}

	/*
	* make the region data accessable.
	*/

	{
		PSCAN   pscn = (PSCAN)((REGION *)prgn + 1);
		ULONG   cscn = rgn.cScans;

		LONG    lPrevBottom = NEG_INFINITY;
		LONG    lPrevRight;

		while (cscn--)
		{
			LONG yTop;
			LONG yBottom;
			LONG cWalls;
			LONG cWalls2;
			LONG left;
			LONG right;
			ULONG iWall = 0;

			move(yTop, (PBYTE)pscn + offsetof(SCAN, yTop));
			move(yBottom, (PBYTE)pscn + offsetof(SCAN, yBottom));
			move(cWalls, (PBYTE)pscn + offsetof(SCAN, cWalls));

			if (yTop < lPrevBottom)
			{
				dprintf("top(0x%lx) < prev bottom(0x%lx), scan %ld, pscn @ 0x%lx\n",
					yTop, lPrevBottom, rgn.cScans - cscn, (BYTE *)pscn - (BYTE *)prgn);
				bCheck = FALSE;
				break;
			}

			if (yTop > yBottom)
			{
				dprintf("top(0x%lx) > bottom(0x%lx), scan %ld, pscn @ 0x%lx\n",
					yTop, yBottom, rgn.cScans - cscn, (BYTE *)pscn - (BYTE *)prgn);
				bCheck = FALSE;
				break;
			}

			lPrevBottom = yBottom;
			lPrevRight = NEG_INFINITY;

			while ((LONG)iWall < cWalls)
			{
				move(left, (PBYTE)pscn + offsetof(SCAN, ai_x[iWall]));
				move(right, (PBYTE)pscn + offsetof(SCAN, ai_x[iWall + 1]));

				if ((left <= lPrevRight) || (right <= left))
				{
					dprintf("left[i] < left[i+1], pscn @ 0x%lx, iWall = 0x%lx\n",
						(BYTE *)pscn - (BYTE *)prgn, iWall);
					bCheck = FALSE;
					break;
				}

				lPrevRight = right;

				iWall += 2;

				if (CheckControlC())
					return;
			}
			if (!bCheck){
				break;
			}

			move(cWalls2, (PBYTE)pscn + offsetof(SCAN, ai_x[iWall]));

			if (cWalls != cWalls2)
			{
				dprintf("cWalls != cWalls2 @ 0x%lx\n",
					(BYTE *)pscn - (BYTE *)prgn);
				bCheck = FALSE;
				break;
			}

			pscn = (PSCAN)((PBYTE)pscn + (cWalls * sizeof(LONG) + sizeof(SCAN)));

			if ((ULONG_PTR)pscn >= (ULONG_PTR)rgn.pscnTail)
			{
				//dprintf("Went past end of region\n");
				break;
			}
		}
	}

	if (!bCheck){
		dprintf("region check failed\n");
	}else{
		dprintf("region check success\n");
	}
	
}

PSZ gapszBMF[] =
{
	"BMF_ERROR",
	"BMF_1BPP",
	"BMF_4BPP",
	"BMF_8BPP",
	"BMF_16BPP",
	"BMF_24BPP",
	"BMF_32BPP",
	"BMF_4RLE",
	"BMF_8RLE"
};

PSZ gapszSTYPE[] =
{
	"STYPE_BITMAP",
	"STYPE_DEVICE",
	"Unused",
	"STYPE_DEVBITMAP",
};

ULONG ulSizeSURFACE()
{
	ULONG surfSize;
	move(surfSize, GetExpression("win32kbase!SURFACE::tSize"));
	if (surfSize != sizeof(SURFACE)){
		DEBUGPRINT("!!!WARNING!!!surface struct has been changed(our=(0x%x, system=0x%x) please check it." \
			"and the information may be wrong\n",
			sizeof(SURFACE), surfSize);
	}
	return sizeof(SURFACE);
}

VOID vPrintSURFACE(PVOID  pv)
{
	SURFACE *pso = (SURFACE *)pv;

	//
	// SURFACE structure
	//

	dprintf("--------------------------------------------------\n");
	dprintf("DHSURF          dhsurf        0x%p\n", pso->so.dhsurf);
	dprintf("HSURF           hsurf         0x%p\n", pso->so.hsurf);
	dprintf("DHPDEV          dhpdev        0x%p\n", pso->so.dhpdev);
	dprintf("HDEV            hdev          0x%p\n", pso->so.hdev);
	dprintf("SIZEL           sizlBitmap.cx 0x%lx\n", pso->so.sizlBitmap.cx);
	dprintf("SIZEL           sizlBitmap.cy 0x%lx\n", pso->so.sizlBitmap.cy);
	dprintf("ULONG           cjBits        0x%lx\n", pso->so.cjBits);
	dprintf("PVOID           pvBits        0x%p\n", pso->so.pvBits);
	dprintf("PVOID           pvScan0       0x%p\n", pso->so.pvScan0);
	dprintf("LONG            lDelta        0x%lx\n", pso->so.lDelta);
	dprintf("ULONG           iUniq         0x%lx\n", pso->so.iUniq);
	dprintf("ULONG           iBitmapFormat 0x%lx, %s\n", pso->so.iBitmapFormat,
		pso->so.iBitmapFormat > BMF_8RLE ? "ERROR" : gapszBMF[pso->so.iBitmapFormat]);

#if 0
	if (pso->DIB.hDIBSection)
	{
		dprintf("USHORT          iType          DIBSECTION\n");

		dprintf("HANDLE          hDIBSection   0x%lx\n", pso->DIB.hDIBSection);
		dprintf("HANDLE          hSecure       0x%lx\n", pso->DIB.hSecure);
		dprintf("DWORD           dwOffset      0x%lx\n", pso->DIB.dwOffset);
	}else{
		dprintf("USHORT          iType         0x%x, %s\n", pso->so.iType,
			pso->so.iType > STYPE_DEVBITMAP ? "ERROR" : gapszSTYPE[pso->so.iType]);
	}
#endif

	dprintf("USHORT          iType         0x%x, %s\n", pso->so.iType,
		pso->so.iType > STYPE_DEVBITMAP ? "ERROR" : gapszSTYPE[pso->so.iType]);


	dprintf("USHORT          fjBitmap      0x%x\n", pso->so.fjBitmap);

#if 0
	dprintf("XDCOBJ*         pdcoAA        0x%ln\n", pso->pdcoAA);
	dprintf("FLONG           flags         0x%lx\n", pso->SurfFlags);
#endif
	dprintf("PPALETTE        ppal          0x%p\n", pso->pPal);

#if 0
	dprintf("PFN_DrvBitBlt   pfnBitBlt     0x%lx\n", pso->pFnBitBlt);
	dprintf("PFN_DrvTextOut  pfnTextOut    0x%lx\n", pso->pFnTextOut);
#endif

#if 0
	if ((pso->so.iType == STYPE_BITMAP) ||
		(pso->so.iType == STYPE_DEVBITMAP)){

		dprintf("HDC             hdc           0x%lx\n", pso->EBitmap.hdc);
		dprintf("ULONG           cRef          0x%lx\n", pso->EBitmap.cRef);
		dprintf("HPALETTE        hpalHint      0x%lx\n", pso->EBitmap.hpalHint);
	}

	if (pso->flags & PDEV_SURFACE)
	{
		dprintf("This is the enabled surface for a PDEV\n");
	}

#endif

	dprintf("--------------------------------------------------\n");
}

VOID Gdidpsurf(
	PVOID pvServer)
{
	char pso[1024];

	move2(pso, pvServer, ulSizeSURFACE());

	dprintf("SURFACE structure at 0x%p:\n", pvServer);
	vPrintSURFACE((PVOID)pso);
}


EXTERN_C
HRESULT CALLBACK
dpsurf(PDEBUG_CLIENT4 Client, PCSTR args)
{
	ULONG_PTR psurf;
	psurf = GetExpression(args);
	if (!psurf) {
		dprintf("Please supply an argument \n");
		return S_OK;
	}

	Gdidpsurf((PVOID *)psurf);
	return S_OK;
}

VOID Gdidpso(
	PVOID pvServer)
{
	char pso[1024];

	//
	// subtract offset of BASE OBJECT FROM SURFOBJ
	//

	pvServer = (PVOID)((PUCHAR)pvServer - /*sizeof(BASEOBJECT)*/0x18);

	move2(pso, pvServer, ulSizeSURFACE());

	dprintf("SURFACE structure at 0x%p:\n", pvServer);
	vPrintSURFACE((PVOID)pso);
}


EXTERN_C
HRESULT CALLBACK
dpso(PDEBUG_CLIENT4 Client, PCSTR args)
{
	ULONG_PTR pso;
	pso = GetExpression(args);
	if (!pso) {
		dprintf("Please supply an argument \n");
		return S_OK;
	}

	Gdidpso((PVOID *)pso);
	return S_OK;
}


ULONG ulSizePALETTE()
{
	return((ULONG) sizeof(PALETTE));
}

VOID
vPrintPALETTE(
	PALETTE *  pvServer
)
{
	PALETTE *ppal;
	char pso[1024];

	move2(pso, pvServer, ulSizePALETTE());

	dprintf("EPALOBJ structure at 0x%p:\n", pvServer);

	ppal = (PALETTE *)pso;

	dprintf("--------------------------------------------------\n");
	dprintf("FLONG      flPal         0x%lx\n", ppal->flPal);
	dprintf("ULONG      cEntries      0x%lx\n", ppal->cEntries);
	dprintf("ULONG      ulTime        0x%lx\n", ppal->ulTime);
	dprintf("HDC        hdcHead       0x%p\n", ppal->hdcHead);
	dprintf("HDEVPPAL   hSelected     0x%p\n", ppal->hSelected.hdev);
	dprintf("ULONG      cRefhpal      0x%lx\n", ppal->cRefhpal);
	dprintf("ULONG      cRefRegular   0x%lx\n", ppal->cRefRegular);
	dprintf("PTRANSLATE ptransFore    0x%p\n", ppal->ptransFore);
	dprintf("PTRANSLATE ptransCurrent 0x%p\n", ppal->ptransCurrent);
	dprintf("PTRANSLATE ptransOld     0x%p\n", ppal->ptransOld);

	// For DirectDraw surfaces, sometimes the colour table is shared
	// with another palette:

	if (ppal->ppalColor != ppal)
	{
		dprintf("PPALETTE   ppalColor     0x%p\n", ppal->ppalColor);
	}

	dprintf("PAL_ULONG  apalColor     0x%p\n", ppal->apalColor);
	dprintf("--------------------------------------------------\n");
}


EXTERN_C
HRESULT CALLBACK
dppal(PDEBUG_CLIENT4 Client, PCSTR args)
{
	PALETTE* ppal;
	ppal = (PALETTE*)GetExpression(args);
	if (!ppal) {
		dprintf("Please supply an argument \n");
		return S_OK;
	}

	Gdidppal(ppal);
	return S_OK;
}

void Gdidppal(
	PALETTE * pvServer)
{
	vPrintPALETTE(pvServer);
}