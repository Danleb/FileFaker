#pragma once

#include <stdbool.h>
#include <Windows.h>

#define FILE_PATH_SIZE 1024

typedef DWORD PID;
typedef int REDIRECTION_HANDLE;
#define INVALID_REDIRECTION_HANDLE -1

typedef struct RedirectionData
{
	REDIRECTION_HANDLE handle;
	BOOL file_from_defined;
	PID pid;
	char file_path_from[FILE_PATH_SIZE];
	char file_path_to[FILE_PATH_SIZE];
} RedirectionData;

extern size_t local_redirections_count;
extern RedirectionData* local_redirection_datas;

bool add_redirection(RedirectionData redirection_data);
bool remove_redirection(REDIRECTION_HANDLE redirection_handle);