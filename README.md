# win32kext

windbg plugin for win32k debugging

## Usage

```
gh     [object handle]               -- HMGR entry of handle (GDI object like DC/BITMAP/PALETTE etc)
uh     [object handle]               -- USER entry of handle (USER object like WINDOW/MENU etc)
duh    [-h]                          -- Dump USER entry of handle (USER object like WINDOW/MENU etc)
dgh    [-h]                          -- Dump HMGR entry of handle (GDI object like DC/BITMAP/PALETTE etc)
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

```
1: kd> !duh -h
Usage: !duh [args]

args list:
-p [process] filter object by process
-t [type id] filter object by type id
   valid type:
   id:1 - Window
   id:2 - Menu
   id:3 - Cursor
   id:4 - DeferWindowPos
   id:5 - WindowHook
   id:6 - MemoryHandle
   id:7 - CPD
   id:8 - AcceleratorTable
   id:9 - CsDde
   id:10 - Conversation
   id:11 - pxs
   id:12 - Monitor
   id:13 - Keyboard
   id:14 - KeyboardLayout
   id:15 - EventHook
   id:16 - Timer
   id:17 - InputContext
   id:18 - HidData
   id:20 - TouchInputInfo
   id:21 - GestureInfo
   id:23 - BaseWindow
example:
!duh
    will dump user object in system
!duh -p 0xffffffff13450080
    will dump user object create by process 0xffffffff13450080
!duh -t 1
    will dump all window object
!duh -t 1 -p 0xffffffff13450080
    will dump all window object create by process 0xffffffff13450080

```

eg

```
1: kd> !duh
Total 0x380 handles
handle=0x00010002 object=0xffffa99640830000 process=0xffffc409f16d4080 type=(01)Window        
handle=0x00010003 object=0xffffa99640850000 process=0xffffc409f16d4080 type=(03)Cursor        
handle=0x00010004 object=0xffffa99640830150 process=0xffffc409f16d4080 type=(01)Window        
handle=0x00010005 object=0xffffa996408500a0 process=0xffffc409f16d4080 type=(03)Cursor        
handle=0x00010006 object=0xffffa996408302a0 process=0xffffc409f16d4080 type=(01)Window  

1: kd> !duh -p 0xffffc409f16d4080 -t 1
Total 0x380 handles
handle=0x00010002 object=0xffffa99640830000 process=0xffffc409f16d4080 type=(01)Window        
handle=0x00010004 object=0xffffa99640830150 process=0xffffc409f16d4080 type=(01)Window        
handle=0x00010006 object=0xffffa996408302a0 process=0xffffc409f16d4080 type=(01)Window        
handle=0x00010008 object=0xffffa996408303f0 process=0xffffc409f16d4080 type=(01)Window        
handle=0x0001000a object=0xffffa99640830540 process=0xffffc409f16d4080 type=(01)Window        
handle=0x0001000c object=0xffffa99640830690 process=0xffffc409f16d4080 type=(01)Window        
user control-c break

1: kd> !duh -t 1
Total 0x380 handles
handle=0x00010002 object=0xffffa99640830000 process=0xffffc409f16d4080 type=(01)Window        w        
handle=0x00020016 object=0xffffa99640832e70 process=0xffffc409f168e240 type=(01)Window        
handle=0x00020018 object=0xffffa99640832d20 process=0xffffc409e96c1080 type=(01)Window        
handle=0x0002001a object=0xffffa99640832bd0 process=0xffffc409e96c1080 type=(01)Window        
handle=0x0003001c object=0xffffa99640832a80 process=0xffffc409f168e240 type=(01)Window        
user control-c break


```

```
1: kd> !dgh -h
Usage: !dgh [args]

args list:
-p [process] filter object by process
-t [type id] filter object by type id
   valid type:
   id:1 - DC
   id:2 - ColorTransform
   id:4 - Rgn
   id:5 - Bitmap
   id:7 - Path
   id:8 - Palette
   id:9 - ColorSpace
   id:10 - Font
   id:14 - ColorTransform
   id:15 - Sprite
   id:16 - Brush
   id:18 - LogicSurface
   id:19 - Space
   id:21 - ServerMetafile
   id:28 - Driver
   id:138 - Font2
example:
!dgh
    will dump gdi object in system
!dgh -p 0xffffffff13450080
    will dump gdi object create by process 0xffffffff13450080
!dgh -t 5
    will dump all bitmap object
!dgh -t 1 -p 0xffffffff13450080
    will dump all bitmap object create by process 0xffffffff13450080

```

eg

```
1: kd> !dgh -t 5
Handle:0x0085000f Object=0xffffa99640890000 Type=Bitmap(5) entry=0xffffa99640a00168 processx=0x0
Handle:0x0005001d Object=0xffffa99640890580 Type=Bitmap(5) entry=0xffffa99640a002b8 processx=0x0
Handle:0x00050031 Object=0xffffa996408902c0 Type=Bitmap(5) entry=0xffffa99640a00498 processx=0x0
Handle:0x00050032 Object=0xffffa99640890840 Type=Bitmap(5) entry=0xffffa99640a004b0 processx=0x0
Handle:0x00050033 Object=0xffffa99640890b00 Type=Bitmap(5) entry=0xffffa99640a004c8 processx=0x0

1: kd> !dgh
Handle:0x0004000a Object=0xffffa99640602d00 Type=Rgn(4) entry=0xffffa99640a000f0 processx=0x0
Handle:0x0088000b Object=0xffffa996408e0000 Type=Palette(8) entry=0xffffa99640a00108 processx=0x0
Handle:0x0008000c Object=0xffffa996408e0090 Type=Palette(8) entry=0xffffa99640a00120 processx=0x0
Handle:0x0008000d Object=0xffffa996408e0120 Type=Palette(8) entry=0xffffa99640a00138 processx=0x0
Handle:0x0008000e Object=0xffffa996408e01b0 Type=Palette(8) entry=0xffffa99640a00150 processx=0x0

```



## Supported system

!!!ONLY TEST ON!!!
windows 10 1803 1903 64bits

why not 32 bits?

Guys you need change your Computer

## TODO

- [ ] DC OBJECT
- [ ] BRUSH OBJECT
- [ ] WINDOW OBJECT
- [ ] MENU OBJECT
- [ ] PDEV OBJECT
- [x] DUMP HANDLE
- [ ] FONT OBJECT
- [ ] DCOMP OBJECT

## Thanks to

[SimpleOpt](https://github.com/brofield/simpleopt)
