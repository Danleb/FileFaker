#pragma once
#include <stddef.h>
#include <Windows.h>

#include "redirections_manager.h"

#define PIPE_NAME_SIZE 256
extern const char CLIENT_READY_MESSAGE[];

typedef struct MessagingData
{
	PID pid;
	HANDLE pipe_read_handle;
	HANDLE pipe_write_handle;
	CHAR pipe_server_read_name[PIPE_NAME_SIZE];
	CHAR pipe_server_write_name[PIPE_NAME_SIZE];
} MessagingData;

typedef enum ServerCommandType
{
	AddRedirection,
	RemoveRedirection
} ServerCommandType;

typedef struct ServerMessageData
{
	ServerCommandType command_type;
	RedirectionData redirection_data;
} ServerMessageData;

typedef struct ClientMessageData
{
	bool success;
} ClientMessageData;

extern size_t redirection_datas_count;
extern RedirectionData* redirection_datas;

extern size_t injected_processes_count;
extern MessagingData* injected_processes;

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