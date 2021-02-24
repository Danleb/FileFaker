#pragma once
#include <stdio.h>
#include <Windows.h>

extern FILE* (*fopen_Original)(char const*, char const*);
extern errno_t(*fopen_s_Original)(FILE** _Stream, char const* _FileName, char const* _Mode);
extern FILE* (*_fsopen_Original)(char const* _FileName, char const* _Mode, int _ShFlag);
extern FILE* (*_wfsopen_Original)(wchar_t const* _FileName, wchar_t const* _Mode, int _ShFlag);
extern FILE* (*_wfopen_Original)(wchar_t const* _FileName, wchar_t const* _Mode);
extern HANDLE(*CreateFileA_Original)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
extern HANDLE(*CreateFileW_Original)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

FILE* fopen_fake(char const* fileName, char const* mode);
errno_t fopen_s_fake(FILE** _Stream, char const* _FileName, char const* _Mode);
FILE* _fsopen_fake(char const* _FileName, char const* _Mode, int _ShFlag);
FILE* _wfsopen_fake(wchar_t const* _FileName, wchar_t const* _Mode, int _ShFlag);
FILE* _wfopen_fake(wchar_t const* _FileName, wchar_t const* _Mode);
HANDLE CreateFileA_fake(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
);
HANDLE CreateFileW_fake(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
);
