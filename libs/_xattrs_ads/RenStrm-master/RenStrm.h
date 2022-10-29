#pragma once

// ---------------------------------------------------------------------------
// NTFS Alternate Data Stream   Rename Utility
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// 
// Written 2018-6-11 by Hernán Di Pietro.
// ---------------------------------------------------------------------------

#include <windows.h>
#include <cstdio>
#include <memory>
#include <strsafe.h>

// ---------------------------------------------------------------------------
// NT Internal Declarations
// ---------------------------------------------------------------------------

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

using NTSTATUS = LONG;

typedef struct _IO_STATUS_BLOCK {
    union
    {
        NTSTATUS Status;
        PVOID    Pointer;
    } DUMMYUNIONNAME;
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

enum FILE_INFORMATION_CLASS {
    FileRenameInformation = 10
};

typedef struct _FILE_RENAME_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

using NTSETINFORMATIONFILE = NTSTATUS(__stdcall *) (HANDLE FileHandle,
    PIO_STATUS_BLOCK IoStatusBlock,
    void * FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass);

using RTLNTSTATUSTODOSERROR = ULONG(WINAPI *) (_In_ NTSTATUS Status);

// ---------------------------------------------------------------------------
// Win32 HANDLE Wrapper
// ---------------------------------------------------------------------------

class UniqueHANDLE
{
    HANDLE m_handle = NULL;

public:
    explicit UniqueHANDLE(HANDLE h) : m_handle(h) {};
    UniqueHANDLE(UniqueHANDLE&) = delete;
    UniqueHANDLE& operator= (const UniqueHANDLE&) = delete;
    
    operator HANDLE() const
    {
        return m_handle;        
    }

    ~UniqueHANDLE()
    {
        if (m_handle)
            CloseHandle(m_handle);
    }
};