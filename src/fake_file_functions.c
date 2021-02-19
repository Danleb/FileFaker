#include <stdio.h>
#include <Windows.h>
#include <string.h>

#include "fake_file_functions.h"
#include "file_redirector.h"

#define FILE_NAME_BUFFER_SIZE 2048

void get_file_name(const char* original, char* final_file_name)
{
	for (size_t i = 0; i < local_redirections_count; ++i)
	{
		const RedirectionData* redirection_data = &local_redirection_datas[i];
		if (!redirection_data->file_from_defined ||
			strcmp(redirection_data->file_path_from, original) == 0)
		{
			strcpy(final_file_name, redirection_data->file_path_to);
			break;
		}
	}
}

HFILE OpenFile_fake(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(lpFileName, final_file_name);
	return OpenFile(lpFileName, lpReOpenBuff, uStyle);
}


FILE* fopen_fake(char const* fileName, char const* mode)
{
	CHAR final_file_name[FILE_NAME_BUFFER_SIZE];
	get_file_name(fileName, final_file_name);
	return fopen(final_file_name, mode);
}