#pragma once
#include <stddef.h>
#include <Windows.h>

typedef DWORD PID;
typedef int REDIRECTION_HANDLE;

typedef struct InjectData
{
	PID pid;
	char pipe_name[1024];
	HANDLE pipe_write_handle;
};

typedef struct RedirectionData
{	
	REDIRECTION_HANDLE handle;
	PID pid;
	char* file_path_from;
	char* file_path_to;
} RedirectionData;

extern RedirectionData* redirection_datas;
extern size_t redirection_datas_count;

extern PID* injected_processes;
extern size_t injected_processes_count;

REDIRECTION_HANDLE redirect_files_io(PID pid, char* file_path_from);
REDIRECTION_HANDLE redirect_file_io(PID pid, char* file_path_from, char* file_path_to);

void restore_file_io(size_t pid, REDIRECTION_HANDLE handle);

void initialize();