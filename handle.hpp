
typedef struct _ENTRY
{
	ULONG_PTR HandleNextFree;
	ULONG ProcessIdOrSome;
	USHORT DirOrType;
	UCHAR Type;
	UCHAR field_F;
	ULONG_PTR field_10;
}ENTRY, *PENTRY;

typedef struct _LOOKUP_ENTRY
{
	PVOID pLock;
	PVOID pObject;
}LOOKUP_ENTRY, *PLOOKUP_ENTRY;

typedef struct _ENTRYDATALOOKUPTABLE
{
	PLOOKUP_ENTRY *ppLookupEntries;
	ULONG Counts;
	LOOKUP_ENTRY LookupEntries[1];
}ENTRYDATALOOKUPTABLE, *PENTRYDATALOOKUPTABLE;

typedef struct _HANDLEENTRYTABLE
{
	PENTRY pEntries;
	ULONG Counts;
	ULONG LastFreeIndex;
	ULONG CurDirIndex;
	ULONG CurLookupIndex;
	PENTRYDATALOOKUPTABLE pEntryDataLookupTable;
	//ENTRY Entries[1];
}HANDLEENTRYTABLE, *PHANDLEENTRYTABLE;

typedef struct _HANDLEENTRYDIR
{
	UCHAR bFilled;
	UCHAR field_1;
	USHORT DirCounts;
	ULONG field_4;
	PHANDLEENTRYTABLE pEntryTable[0x100];
	ULONG MaxDirGdiHandleCount;
	ULONG field_80C;
}HANDLEENTRYDIR, *PHANDLEENTRYDIR;


typedef struct _HANDLEMGR
{
	ULONG TotalGdiHandleCounts;
	ULONG CurrentGdiHandleCounts;
	ULONG MaxGdiHandleCountSystem;
	ULONG field_C;
	PHANDLEENTRYDIR pHandleEntryDir;
	PULONG pHandleValueBackup;
}HANDLEMGR, *PHANDLEMGR;


typedef struct _HANDLEENTRY
{
	ULONG_PTR ObjectOffset;
	ULONG_PTR ThreadOrProcessId;
	PVOID rpDisk;
	UCHAR Type;
	UCHAR field_19;
	USHORT wUniq;
	ULONG field_1C;
}HANDLEENTRY, *PHANDLEENTRY;


typedef struct tagSHAREDINFO
{
	PVOID psi;
	PHANDLEENTRY aheList;
	ULONG aheSize;
	ULONG field_14;
	PVOID dispInfo;
}SHAREDINFO, *PSHAREDINFO;


typedef struct _KERNEL_HANDLEENTRY
{
	PVOID pObject;
	PVOID pOwner;
	PVOID field_10;
}KERNEL_HANDLEENTRY, *PKERNEL_HANDLEENTRY;

typedef struct _USER_TYPE_DESC
{
	PVOID fnDestroy;
	ULONG Tag;
	ULONG CreateFlag;
	ULONG field_10;
	ULONG field_14;
}USER_TYPE_DESC, *PUSER_TYPE_DESC;




