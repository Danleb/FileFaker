#include <stdio.h>
#include <Windows.h>
#include <string.h>

#include "hook_file_functions.h"
#include "redirections_manager.h"

#define FILE_NAME_BUFFER_SIZE 2048

FILE* (*fopen_Original)(char const*, char const*);
HANDLE(*CreateFileA_Original)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
HANDLE(*CreateFileW_Original)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

void get_file_name(const char* original, char* final_file_name)
{
	for (size_t i = 0; i < local_redirections_count; ++i)
	{
		const RedirectionData* redirection_data = &local_redirection_datas[i];
		if (!redirection_data->file_from_defined ||
			strcmp(redirection_data->file_path_from, original) == 0)
		{
			strcpy(final_file_name, redirection_data->file_path_to);
			return;
		}
	}

	strcpy(final_file_name, original);
}

FILE* fopen_fake(char const* fileName, char const* mode)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(fileName, final_file_name);
	return fopen_Original(final_file_name, mode);
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
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(lpFileName, final_file_name);
	return CreateFileW_Original(final_file_name,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}