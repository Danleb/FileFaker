#include <stdio.h>
#include <Windows.h>
#include <string.h>

#include "hook_file_functions.h"
#include "redirections_manager.h"

#define FILE_NAME_BUFFER_SIZE 2048

FILE* (*fopen_Original)(char const*, char const*);
errno_t(*fopen_s_Original)(FILE** _Stream, char const* _FileName, char const* _Mode);
FILE* (*_fsopen_Original)(char const* _FileName, char const* _Mode, int _ShFlag);
HANDLE(*CreateFileA_Original)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
FILE* (*_wfsopen_Original)(wchar_t const* _FileName, wchar_t const* _Mode, int _ShFlag);
FILE* (*_wfopen_Original)(wchar_t const* _FileName, wchar_t const* _Mode);
errno_t(*_wfopen_s_Original)(FILE** _Stream, wchar_t const* _FileName, wchar_t const* _Mode);
FILE* (*freopen_Original)(char const* _FileName, char const* _Mode, FILE* _Stream);
FILE* (*_wfreopen_Original)(wchar_t const* _FileName, wchar_t const* _Mode, FILE* _OldStream);
HANDLE(*CreateFileW_Original)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

void get_file_name_w(const wchar_t* original, wchar_t* final_file_name)
{
	for (size_t i = 0; i < local_redirections_count; ++i)
	{
		const RedirectionData* redirection_data = &local_redirection_datas[i];
		if (!redirection_data->file_from_defined ||
			wcscmp(redirection_data->file_path_from, original) == 0)
		{
			wcscpy(final_file_name, redirection_data->file_path_to);
			return;
		}
	}
	wcscpy(final_file_name, original);
}

void get_file_name(const char* original, char* final_file_name)
{
	int wide_count = MultiByteToWideChar(CP_ACP, NULL, original, -1, NULL, 0);

	wchar_t* original_w = malloc(sizeof(wchar_t) * wide_count);
	if (original_w == NULL)
	{
		return;
	}
	MultiByteToWideChar(CP_ACP, NULL, original, -1, original_w, wide_count);

	wchar_t* final_file_name_w = malloc(sizeof(wchar_t) * FILE_NAME_BUFFER_SIZE);
	if (final_file_name_w == NULL)
	{
		return;
	}
	get_file_name_w(original_w, final_file_name_w);

	WideCharToMultiByte(CP_ACP, NULL, final_file_name_w, -1, final_file_name, FILE_NAME_BUFFER_SIZE, NULL, NULL);
	free(original_w);
	free(final_file_name_w);
}

FILE* fopen_fake(char const* fileName, char const* mode)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(fileName, final_file_name);
	return fopen_Original(final_file_name, mode);
}

errno_t fopen_s_fake(FILE** _Stream, char const* _FileName, char const* _Mode)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(_FileName, final_file_name);
	return fopen_s_Original(_Stream, final_file_name, _Mode);
}

FILE* _fsopen_fake(char const* _FileName, char const* _Mode, int _ShFlag)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(_FileName, final_file_name);
	return _fsopen_Original(final_file_name, _Mode, _ShFlag);
}

HANDLE CreateFileA_fake(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(lpFileName, final_file_name);
	return CreateFileA_Original(final_file_name,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

FILE* _wfsopen_fake(wchar_t const* _FileName, wchar_t const* _Mode, int _ShFlag)
{
	WCHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name_w(_FileName, final_file_name);
	return _wfsopen_Original(final_file_name, _Mode, _ShFlag);
}

FILE* _wfopen_fake(wchar_t const* _FileName, wchar_t const* _Mode)
{
	WCHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name_w(_FileName, final_file_name);
	return _wfopen_Original(final_file_name, _Mode);
}

errno_t _wfopen_s_fake(FILE** _Stream, wchar_t const* _FileName, wchar_t const* _Mode)
{
	WCHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name_w(_FileName, final_file_name);
	return _wfopen_s_Original(_Stream, final_file_name, _Mode);
}

FILE* freopen_fake(char const* _FileName, char const* _Mode, FILE* _Stream)
{
	char final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(_FileName, final_file_name);
	return freopen_Original(final_file_name, _Mode, _Stream);
}

FILE* _wfreopen_fake(wchar_t const* _FileName, wchar_t const* _Mode, FILE* _OldStream)
{
	WCHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name_w(_FileName, final_file_name);
	return _wfreopen_Original(final_file_name, _Mode, _OldStream);
}

HANDLE CreateFileW_fake(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
)
{
	WCHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name_w(lpFileName, final_file_name);
	return CreateFileW_Original(final_file_name,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}