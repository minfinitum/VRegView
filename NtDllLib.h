#pragma once

#include <windows.h>
#include <string>
#include <ntsecapi.h>

//#include <ntstatus.h>
//#include <winternl.h>

// Structure Definitions

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
    (p)->RootDirectory = r;                           \
    (p)->Attributes = a;                              \
    (p)->ObjectName = n;                              \
    (p)->SecurityDescriptor = s;                      \
    (p)->SecurityQualityOfService = NULL;             \
}

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)
#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)
#define APPLICATION_ERROR_MASK           0x20000000
#define ERROR_SEVERITY_SUCCESS           0x00000000
#define ERROR_SEVERITY_INFORMATIONAL     0x40000000
#define ERROR_SEVERITY_WARNING           0x80000000
#define ERROR_SEVERITY_ERROR             0xC0000000
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L) // ntsubauth
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)
#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_NOT_IMPLEMENTED           ((NTSTATUS)0xC0000002L)
#define STATUS_INVALID_INFO_CLASS        ((NTSTATUS)0xC0000003L)
#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
//#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)
#define STATUS_NO_SUCH_DEVICE            ((NTSTATUS)0xC000000EL)
#define STATUS_NO_SUCH_FILE              ((NTSTATUS)0xC000000FL)
#define STATUS_INVALID_DEVICE_REQUEST    ((NTSTATUS)0xC0000010L)
#define STATUS_END_OF_FILE               ((NTSTATUS)0xC0000011L)
#define STATUS_NO_MEDIA_IN_DEVICE        ((NTSTATUS)0xC0000013L)
#define STATUS_UNRECOGNIZED_MEDIA        ((NTSTATUS)0xC0000014L)
#define STATUS_MORE_PROCESSING_REQUIRED  ((NTSTATUS)0xC0000016L)
#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)
#define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023L)
#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)
#define NT_STATUS_OBJECT_PATH_SYNTAX_BAD ((NTSTATUS)0xC000003BL)

// Valid values for the Attributes field
//
// This handle can be inherited by child processes of the current process.
#define OBJ_INHERIT				0x00000002L

// This flag only applies to objects that are named within the Object Manager.
// By default, such objects are deleted when all open handles to them are closed.
// If this flag is specified, the object is not deleted when all open handles are
// closed. Drivers can use ZwMakeTemporaryObject to delete permanent objects.
#define OBJ_PERMANENT			0x00000010L

// Only a single handle can be open for this object.
#define OBJ_EXCLUSIVE			0x00000020L

// If this flag is specified, a case-insensitive comparison is used when
// matching the ObjectName parameter against the names of existing objects.
// Otherwise, object names are compared using the default system settings.
#define OBJ_CASE_INSENSITIVE	0x00000040L

typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileCopyOnWriteInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileOleClassIdInformation,
    FileOleStateBitsInformation,
    FileNetworkOpenInformation,
    FileObjectIdInformation,
    FileOleAllInformation,
    FileOleDirectoryInformation,
    FileContentIndexInformation,
    FileInheritContentIndexInformation,
    FileOleInformation,
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_FOR_RECOVERY                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG  NextEntryOffset;
    ULONG  FileIndex;
    LARGE_INTEGER  CreationTime;
    LARGE_INTEGER  LastAccessTime;
    LARGE_INTEGER  LastWriteTime;
    LARGE_INTEGER  ChangeTime;
    LARGE_INTEGER  EndOfFile;
    LARGE_INTEGER  AllocationSize;
    ULONG  FileAttributes;
    ULONG  FileNameLength;
    ULONG  EaSize;
    CCHAR  ShortNameLength;
    WCHAR  ShortName[12];
    WCHAR  FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Unknown;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaInformationLength;
    WCHAR FileName[1];
} FILE_FULL_DIRECTORY_INFORMATION, *PFILE_FULL_DIRECTORY_INFORMATION;

typedef struct _FILE_BOTH_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Unknown;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaInformationLength;
    UCHAR AlternateNameLength;
    WCHAR AlternateName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIRECTORY_INFORMATION, *PFILE_BOTH_DIRECTORY_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Unknown;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct FILE_BASIC_INFORMATION {
    LARGE_INTEGER  CreationTime;
    LARGE_INTEGER  LastAccessTime;
    LARGE_INTEGER  LastWriteTime;
    LARGE_INTEGER  ChangeTime;
    ULONG  FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef struct _FILE_STREAM_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

// ----------------------------------------------------------------------------
// winternl.h

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef
VOID
(NTAPI *PIO_APC_ROUTINE) (
                          IN PVOID ApcContext,
                          IN PIO_STATUS_BLOCK IoStatusBlock,
                          IN ULONG Reserved
                          );

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;


// Valid values for the Attributes field
//
// This handle can be inherited by child processes of the current process.
#define OBJ_INHERIT				0x00000002L

// This flag only applies to objects that are named within the Object Manager. 
// By default, such objects are deleted when all open handles to them are closed. 
// If this flag is specified, the object is not deleted when all open handles are 
// closed. Drivers can use ZwMakeTemporaryObject to delete permanent objects.
#define OBJ_PERMANENT			0x00000010L

// Only a single handle can be open for this object.
#define OBJ_EXCLUSIVE			0x00000020L

// If this flag is specified, a case-insensitive comparison is used when 
// matching the ObjectName parameter against the names of existing objects. 
// Otherwise, object names are compared using the default system settings.
#define OBJ_CASE_INSENSITIVE	0x00000040L

// If an object handle, with this flag set, is passed to a routine that opens 
// objects and if the object is a symbolic link object, the routine should open 
// the symbolic link object itself, rather than the object that the symbolic link 
// refers to (which is the default behavior).
#define OBJ_OPENLINK            0x00000100L

// =================================================================
// Key query structures
// =================================================================
typedef struct _KEY_BASIC_INFORMATION 
{
    LARGE_INTEGER LastWriteTime;// The last time the key or any of its values changed.
    ULONG TitleIndex;			// Device and intermediate drivers should ignore this member.
    ULONG NameLength;			// The size in bytes of the following name, including the zero-terminating character.
    WCHAR Name[1];				// A zero-terminated Unicode string naming the key.
} KEY_BASIC_INFORMATION;
typedef KEY_BASIC_INFORMATION *PKEY_BASIC_INFORMATION;

typedef struct _KEY_FULL_INFORMATION 
{
    LARGE_INTEGER LastWriteTime;// The last time the key or any of its values changed.
    ULONG TitleIndex;			// Device and intermediate drivers should ignore this member.
    ULONG ClassOffset;			// The offset from the start of this structure to the Class member.
    ULONG ClassLength;			// The number of bytes in the Class name.
    ULONG SubKeys;				// The number of subkeys for the key.
    ULONG MaxNameLen;			// The maximum length of any name for a subkey.
    ULONG MaxClassLen;			// The maximum length for a Class name.
    ULONG Values;				// The number of value entries.
    ULONG MaxValueNameLen;		// The maximum length of any value entry name.
    ULONG MaxValueDataLen;		// The maximum length of any value entry data field.
    WCHAR Class[1];				// A zero-terminated Unicode string naming the class of the key.
} KEY_FULL_INFORMATION;
typedef KEY_FULL_INFORMATION *PKEY_FULL_INFORMATION;

typedef struct _KEY_NODE_INFORMATION 
{
    LARGE_INTEGER LastWriteTime;// The last time the key or any of its values changed.
    ULONG TitleIndex;			// Device and intermediate drivers should ignore this member.
    ULONG ClassOffset;			// The offset from the start of this structure to the Class member.
    ULONG ClassLength;			// The number of bytes in the Class name.
    ULONG NameLength;			// The size in bytes of the following name, including the zero-terminating character.
    WCHAR Name[1];				// A zero-terminated Unicode string naming the key.
} KEY_NODE_INFORMATION;
typedef KEY_NODE_INFORMATION *PKEY_NODE_INFORMATION;

// end_wdm
typedef struct _KEY_NAME_INFORMATION 
{
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;
typedef KEY_NAME_INFORMATION *PKEY_NAME_INFORMATION;

// begin_wdm
typedef enum _KEY_INFORMATION_CLASS 
{
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
    // end_wdm
    ,
    KeyNameInformation
    // begin_wdm
} KEY_INFORMATION_CLASS;

typedef struct _KEY_WRITE_TIME_INFORMATION 
{
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION;
typedef KEY_WRITE_TIME_INFORMATION *PKEY_WRITE_TIME_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS 
{
    KeyWriteTimeInformation
} KEY_SET_INFORMATION_CLASS;

typedef struct _KEY_VALUE_BASIC_INFORMATION 
{
    ULONG TitleIndex;	// Device and intermediate drivers should ignore this member.
    ULONG Type;			// The system-defined type for the registry value in the 
    // Data member (see the values above).
    ULONG NameLength;	// The size in bytes of the following value entry name, 
    // including the zero-terminating character.
    WCHAR Name[1];		// A zero-terminated Unicode string naming a value entry of 
    // the key.
} KEY_VALUE_BASIC_INFORMATION;
typedef KEY_VALUE_BASIC_INFORMATION *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION 
{
    ULONG TitleIndex;	// Device and intermediate drivers should ignore this member.
    ULONG Type;			// The system-defined type for the registry value in the 
    // Data member (see the values above).
    ULONG DataOffset;	// The offset from the start of this structure to the data 
    // immediately following the Name string.
    ULONG DataLength;	// The number of bytes of registry information for the value 
    // entry identified by Name.
    ULONG NameLength;	// The size in bytes of the following value entry name, 
    // including the zero-terminating character.
    WCHAR Name[1];		// A zero-terminated Unicode string naming a value entry of 
    // the key.
    //	WCHAR Data[1];      // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION;
typedef KEY_VALUE_FULL_INFORMATION *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION 
{
    ULONG TitleIndex;	// Device and intermediate drivers should ignore this member.
    ULONG Type;			// The system-defined type for the registry value in the 
    // Data member (see the values above).
    ULONG DataLength;	// The size in bytes of the Data member.
    UCHAR Data[1];		// A value entry of the key.
} KEY_VALUE_PARTIAL_INFORMATION;
typedef KEY_VALUE_PARTIAL_INFORMATION *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_ENTRY 
{
    PUNICODE_STRING ValueName;
    ULONG           DataLength;
    ULONG           DataOffset;
    ULONG           Type;
} KEY_VALUE_ENTRY;
typedef KEY_VALUE_ENTRY *PKEY_VALUE_ENTRY;

typedef enum _KEY_VALUE_INFORMATION_CLASS 
{
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_MULTIPLE_VALUE_INFORMATION 
{
    PUNICODE_STRING	ValueName;
    ULONG			DataLength;
    ULONG			DataOffset;
    ULONG			Type;
} KEY_MULTIPLE_VALUE_INFORMATION;
typedef KEY_MULTIPLE_VALUE_INFORMATION *PKEY_MULTIPLE_VALUE_INFORMATION;


// NtCreateKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtCreateKey)
(
 IN HANDLE				KeyHandle, 
 IN ULONG				DesiredAccess, 
 IN POBJECT_ATTRIBUTES	ObjectAttributes,
 IN ULONG				TitleIndex, 
 IN PUNICODE_STRING		Class,			/* optional*/
 IN ULONG				CreateOptions, 
 OUT PULONG				Disposition		/* optional*/
 );

// NtOpenKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtOpenKey)
(
 IN HANDLE				KeyHandle,
 IN ULONG				DesiredAccess,
 IN POBJECT_ATTRIBUTES	ObjectAttributes
 );

// NtClose
typedef NTSTATUS (STDAPICALLTYPE * FPNtClose)
(
 IN HANDLE KeyHandle
 );

// NtSetValueKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtSetValueKey)
(
 IN HANDLE			KeyHandle,
 IN PUNICODE_STRING	ValueName,
 IN ULONG			TitleIndex,			/* optional */
 IN ULONG			Type,
 IN PVOID			Data,
 IN ULONG			DataSize
 );

// NtQueryValueKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtQueryValueKey)
(
 // Is the handle, returned by a successful 
 // call to NtCreateKey or NtOpenKey, of key 
 // for which value entries are to be read.
 IN HANDLE			KeyHandle,		 
 IN PUNICODE_STRING	ValueName,
 IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
 OUT PVOID			KeyValueInformation,
 IN ULONG			Length,
 OUT PULONG			ResultLength
 );

// NtQueryKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtQueryKey)
(
 IN HANDLE	KeyHandle,
 IN KEY_INFORMATION_CLASS KeyInformationClass,
 OUT PVOID	KeyInformation,
 IN ULONG	KeyInformationLength,
 OUT PULONG	ResultLength
 );

// NtEnumerateKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtEnumerateKey)
(
 IN HANDLE	KeyHandle,
 IN ULONG	Index,
 IN KEY_INFORMATION_CLASS KeyInformationClass,
 OUT PVOID	KeyInformation,
 IN ULONG	KeyInformationLength,
 OUT PULONG	ResultLength
 );

// NtEnumerateValueKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtEnumerateValueKey)
(
 IN HANDLE	KeyHandle,
 IN ULONG	Index,
 IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
 OUT PVOID	KeyValueInformation,
 IN ULONG	KeyValueInformationLength,
 OUT PULONG	ResultLength
 );

// NtDeleteValueKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtDeleteValueKey)
(
 IN HANDLE			KeyHandle,
 IN PUNICODE_STRING	ValueName
 );

// NtDeleteKey
typedef NTSTATUS (STDAPICALLTYPE * FPNtDeleteKey)
(
 IN HANDLE KeyHandle
 );

// RtlInitUnicodeString
typedef void (STDAPICALLTYPE * FPRtlInitUnicodeString)
(
 IN OUT PUNICODE_STRING DestinationString,
 IN LPCWSTR SourceString
 );

#define		DLL_NAME_NTDLL			L"ntdll.dll"


class NtDllLib
{
public:
    NtDllLib()
    {
        m_hDll = NULL;
        unloadDll();
    }

    ~NtDllLib()
    {
        unloadDll();
    }

    static NtDllLib * NtDllLib::instance()
    {
        static NtDllLib ntdlllib;
        return &ntdlllib;
    }

    bool isLoaded()
    {
        return (m_hDll != NULL);
    }

    bool loadDll(const wchar_t * szPath)
    {
        // already opened
        if (m_hDll)
            unloadDll();

        // Validate OS
        

        std::wstring location = szPath;
        if (location[location.length() -1] != L'\\' && location[location.length() -1] != L'/')
            location += L"\\";

        location += DLL_NAME_NTDLL;
        if (GetFileAttributesW(location.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            location = L".\\";
            location += DLL_NAME_NTDLL;
        }

        m_hDll = LoadLibraryW(location.c_str());
        if ( ! m_hDll)
            return false;

        m_fpNtCreateKey = (FPNtCreateKey)GetProcAddress(m_hDll, "NtCreateKey");
        m_fpNtOpenKey = (FPNtOpenKey)GetProcAddress(m_hDll, "NtOpenKey");
        m_fpNtClose = (FPNtClose)GetProcAddress(m_hDll, "NtClose");

        m_fpNtSetValueKey = (FPNtSetValueKey)GetProcAddress(m_hDll, "NtSetValueKey");
        m_fpNtQueryValueKey = (FPNtQueryValueKey)GetProcAddress(m_hDll, "NtQueryValueKey");
        m_fpNtQueryKey = (FPNtQueryKey)GetProcAddress(m_hDll, "NtQueryKey");
        m_fpNtEnumerateKey = (FPNtEnumerateKey)GetProcAddress(m_hDll, "NtEnumerateKey");
        m_fpNtEnumerateValueKey = (FPNtEnumerateValueKey)GetProcAddress(m_hDll, "NtEnumerateValueKey");

        m_fpNtDeleteValueKey = (FPNtDeleteValueKey)GetProcAddress(m_hDll, "NtDeleteValueKey");
        m_fpNtDeleteKey = (FPNtDeleteKey)GetProcAddress(m_hDll, "NtDeleteKey");

        m_fpRtlInitUnicodeString = (FPRtlInitUnicodeString)GetProcAddress(m_hDll, "RtlInitUnicodeString");

        bool bRetVal = true;

        if(m_fpNtCreateKey == NULL || m_fpNtOpenKey == NULL ||
            m_fpNtClose == NULL)
            bRetVal = false;

        if(m_fpNtSetValueKey == NULL || m_fpNtQueryValueKey == NULL || m_fpNtQueryKey == NULL || m_fpNtEnumerateValueKey == NULL || m_fpNtEnumerateKey == NULL)
            bRetVal = false;

        if(m_fpNtDeleteValueKey == NULL)
            bRetVal = false;

        if(m_fpNtDeleteKey == NULL)
            bRetVal = false;

        if(m_fpRtlInitUnicodeString == NULL)
            bRetVal = false;

        return bRetVal;
    }

    void unloadDll()
    {
        m_fpNtCreateKey = NULL;
        m_fpNtOpenKey = NULL;
        m_fpNtClose = NULL;

        m_fpNtSetValueKey = NULL;
        m_fpNtQueryValueKey = NULL;
        m_fpNtQueryKey = NULL;
        m_fpNtEnumerateKey = NULL;
        m_fpNtEnumerateValueKey = NULL;

        m_fpNtDeleteValueKey = NULL;
        m_fpNtDeleteKey = NULL;

        m_fpRtlInitUnicodeString = NULL;

        if(m_hDll != NULL)
            FreeLibrary(m_hDll);
        m_hDll = NULL;
    }

    NTSTATUS NtCreateKey(HANDLE KeyHandle, ULONG DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
        ULONG TitleIndex, PUNICODE_STRING Class, ULONG CreateOptions, PULONG Disposition)
    {
        if(!m_fpNtCreateKey)
            return -1;
        return m_fpNtCreateKey(KeyHandle, DesiredAccess, ObjectAttributes, TitleIndex, Class, CreateOptions, Disposition);
    }

    NTSTATUS NtOpenKey(HANDLE KeyHandle, ULONG DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes)
    {
        if(!m_fpNtOpenKey)
            return -1;
        return m_fpNtOpenKey(KeyHandle, DesiredAccess, ObjectAttributes);
    }

    NTSTATUS NtClose(HANDLE KeyHandle)
    {
        if(!m_fpNtClose)
            return -1;
        return m_fpNtClose(KeyHandle);
    }

    NTSTATUS NtSetValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, ULONG TitleIndex, ULONG Type, PVOID Data, ULONG	DataSize)
    {
        if(!m_fpNtSetValueKey)
            return -1;
        return m_fpNtSetValueKey(KeyHandle, ValueName, TitleIndex, Type, Data, DataSize);
    }

    NTSTATUS NtQueryValueKey(HANDLE KeyHandle, PUNICODE_STRING ValueName, KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        PVOID KeyValueInformation, ULONG Length, PULONG ResultLength)
    {
        if(!m_fpNtQueryValueKey)
            return -1;
        return m_fpNtQueryValueKey(KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength);
    }

    NTSTATUS NtQueryKey(HANDLE KeyHandle, KEY_INFORMATION_CLASS KeyInformationClass, 
        PVOID KeyInformation, ULONG KeyInformationLength, PULONG ResultLength)
    {
        if(!m_fpNtQueryKey)
            return -1;
        return m_fpNtQueryKey(KeyHandle, KeyInformationClass, KeyInformation, KeyInformationLength, ResultLength);
    }

    NTSTATUS NtEnumerateKey(HANDLE KeyHandle, ULONG Index, KEY_INFORMATION_CLASS KeyInformationClass, 
        PVOID KeyInformation, ULONG KeyInformationLength, PULONG ResultLength)
    {
        if(!m_fpNtEnumerateKey)
            return -1;
        return m_fpNtEnumerateKey(KeyHandle, Index, KeyInformationClass, KeyInformation, KeyInformationLength, ResultLength);
    }

    NTSTATUS NtEnumerateValueKey(HANDLE	KeyHandle, ULONG Index, KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        PVOID KeyValueInformation,ULONG KeyValueInformationLength, PULONG ResultLength)
    {
        if(!m_fpNtEnumerateValueKey)
            return -1;
        return m_fpNtEnumerateValueKey(KeyHandle, Index, KeyValueInformationClass, KeyValueInformation, KeyValueInformationLength, ResultLength);
    }

    NTSTATUS NtDeleteValueKey(HANDLE KeyHandle,PUNICODE_STRING ValueName)
    {
        if(!m_fpNtDeleteValueKey)
            return -1;
        return m_fpNtDeleteValueKey(KeyHandle, ValueName);
    }

    NTSTATUS NtDeleteKey(HANDLE KeyHandle)
    {
        if(!m_fpNtDeleteKey)
            return -1;
        return m_fpNtDeleteKey(KeyHandle);
    }

    void RtlInitUnicodeString(PUNICODE_STRING DestinationString, LPCWSTR SourceString)
    {
        if(!m_fpRtlInitUnicodeString)
            return;
        m_fpRtlInitUnicodeString(DestinationString, SourceString);
    }


private:
    HMODULE m_hDll;

    FPNtCreateKey m_fpNtCreateKey;
    FPNtOpenKey m_fpNtOpenKey;
    FPNtClose m_fpNtClose;

    FPNtSetValueKey m_fpNtSetValueKey;
    FPNtQueryValueKey m_fpNtQueryValueKey;
    FPNtQueryKey m_fpNtQueryKey;
    FPNtEnumerateKey m_fpNtEnumerateKey;
    FPNtEnumerateValueKey m_fpNtEnumerateValueKey;

    FPNtDeleteValueKey m_fpNtDeleteValueKey;
    FPNtDeleteKey m_fpNtDeleteKey;

    FPRtlInitUnicodeString m_fpRtlInitUnicodeString;
};
