#pragma once
#include <stddef.h>
#include <Windows.h>

#define PIPE_NAME_SIZE 256
#define FILE_PATH_SIZE 1024

typedef DWORD PID;
typedef int REDIRECTION_HANDLE;
#define INVALID_REDIRECTION_HANDLE -1

typedef struct ProcessInjectData
{
	PID pid;
	CHAR pipe_server_write_name[PIPE_NAME_SIZE];
	HANDLE pipe_server_write_handle;
	CHAR pipe_server_read_name[PIPE_NAME_SIZE];
	HANDLE pipe_server_read_handle;
} ProcessInjectData;

typedef struct RedirectionData
{
	REDIRECTION_HANDLE handle;
	PID pid;
	char file_path_from[FILE_PATH_SIZE];
	char file_path_to[FILE_PATH_SIZE];
} RedirectionData;

typedef struct ServerMessageData
{
	BOOL file_from_defined;
	CHAR file_from[FILE_PATH_SIZE];
	CHAR file_to[FILE_PATH_SIZE];
} ServerMessageData;

typedef struct ClientMessageData
{
	BOOL success;
} ClientMessageData;

extern size_t redirection_datas_count;
extern RedirectionData* redirection_datas;

extern size_t injected_processes_count;
extern ProcessInjectData* injected_processes;

#ifdef __cplusplus
extern "C" {
#endif

	REDIRECTION_HANDLE redirect_files_io(PID pid, const char* file_path_to);
	REDIRECTION_HANDLE redirect_file_io(PID pid, const char* file_path_from, const char* file_path_to);

	void restore_file_io(REDIRECTION_HANDLE handle);
	bool get_named_pipe_name(PID pid, LPCSTR pipe_name, size_t size, bool serverWrite);

#ifdef __cplusplus
}
#endif