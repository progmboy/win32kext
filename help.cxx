
#include "stdafx.h"


char *gaszHelp[] = {
 "=======================================================================\n"
,"win32k debugger extentions:\n"
,"author: pgboy\n"
,"-----------------------------------------------------------------------\n"
,"\n"
,"gh     [object handle]               -- HMGR entry of handle (GDI object like DC/BITMAP/PALETTE etc)\n"
,"uh     [object handle]               -- USER entry of handle (USER object like WINDOW/MENU etc)\n"
,"dpsurf [SURFACE ptr]                 -- SURFACE\n"
,"dpso   [SURFOBJ ptr]                 -- SURFACE struct from SURFOBJ\n"
,"dr     [REGION ptr]                  -- REGION\n"
,"cr     [REGION ptr]                  -- check REGION\n"
,"dppal  [PALETTE ptr]                 -- PALETTE\n"
,"=======================================================================\n"
,NULL
};

EXTERN_C
HRESULT CALLBACK
help(PDEBUG_CLIENT4 Client, PCSTR args)
{
	for (char **ppsz = gaszHelp; *ppsz; ppsz++)
		dprintf("%s", *ppsz);
	return S_OK;
}
