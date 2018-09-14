# win32kext

windbg plugin for win32k debugging

## Usage

```
gh     [object handle]               -- HMGR entry of handle (GDI object like DC/BITMAP/PALETTE etc)
uh     [object handle]               -- USER entry of handle (USER object like WINDOW/MENU etc)
dpsurf [SURFACE ptr]                 -- SURFACE
dpso   [SURFOBJ ptr]                 -- SURFACE struct from SURFOBJ
dr     [REGION ptr]                  -- REGION
cr     [REGION ptr]                  -- check REGION
dppal  [PALETTE ptr]                 -- PALETTE
help                                 -- show help
```

eg

```
kd> !gh rcx
Object Type         DC(1)
Handle              0x18010665
Object              0xffffa03dc4838010
kd> dq 0xffffa03dc4838010+1f8 l1
ffffa03d`c4838208  ffffa03d`c02a4b00
kd> !dpsurf ffffa03d`c02a4b00
!!!WARNING!!!surface struct has been changed(our=(0x278, system=0x2b8) please check it.and the information may be wrong
SURFACE structure at 0xffffa03dc02a4b00:
--------------------------------------------------
DHSURF          dhsurf        0x0000000000000000
HSURF           hsurf         0x000000000005068a
DHPDEV          dhpdev        0x0000000000000000
HDEV            hdev          0xffffa03dc001c010
SIZEL           sizlBitmap.cx 0x63d
SIZEL           sizlBitmap.cy 0x32
ULONG           cjBits        0x4dfa8
PVOID           pvBits        0x00000000046e0000
PVOID           pvScan0       0x00000000046e0000
LONG            lDelta        0x18f4
ULONG           iUniq         0x940
ULONG           iBitmapFormat 0x6, BMF_32BPP
USHORT          iType         0x0, STYPE_BITMAP
USHORT          fjBitmap      0x811
PPALETTE        ppal          0xffffa03dc3d33e10
--------------------------------------------------
```

## Supported system

!!!ONLY TEST ON!!!
windows 10 1803 64bits

why not 32 bits?

Guys you need change your Computer

## TODO

- [ ] DC OBJECT
- [ ] BRUSH OBJECT
- [ ] WINDOW OBJECT
- [ ] MENU OBJECT
- [ ] PDEV OBJECT
- [ ] DUMP HANDLE
- [ ] FONT OBJECT

## Tanks to

[SimpleOpt](https://github.com/brofield/simpleopt)
