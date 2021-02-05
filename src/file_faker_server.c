#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>

#include "injector_loader.h"
#include "file_faker_server.h"

#define BUFFER_SIZE 2048

size_t redirection_datas_count = 0;
RedirectionData* redirection_datas = NULL;
REDIRECTION_HANDLE redirection_counter = 0;

size_t injected_processes_count = 0;
ProcessInjectData* injected_processes = NULL;

bool initialize_file_faker_client(PID pid, ProcessInjectData* data);
void initialize_pipes(ProcessInjectData* data);

REDIRECTION_HANDLE send_redirection_to(ProcessInjectData* data, const char* file_path_to);
REDIRECTION_HANDLE send_redirection_from_to(ProcessInjectData* data, const char* file_path_from, const char* file_path_to);
REDIRECTION_HANDLE send_redirection(ProcessInjectData* data, ServerMessageData* server_message_data);

REDIRECTION_HANDLE redirect_files_io(PID pid, const char* file_path_to)
{
	ProcessInjectData* process_inject_data = NULL;
	if (!initialize_file_faker_client(pid, process_inject_data))
	{
		printf("Failed to inject to the process.");
		return INVALID_REDIRECTION_HANDLE;
	}
	REDIRECTION_HANDLE redirection_handle = send_redirection_to(pid, file_path_to);
	return redirection_handle;
}

REDIRECTION_HANDLE redirect_file_io(PID pid, const char* file_path_from, const char* file_path_to)
{
	ProcessInjectData process_inject_data;
	if (!initialize_file_faker_client(pid, &process_inject_data))
	{
		printf("Failed to inject to the process.");
		return INVALID_REDIRECTION_HANDLE;
	}
	REDIRECTION_HANDLE redirection_handle = send_redirection_from_to(&process_inject_data, file_path_from, file_path_to);
	return redirection_handle;
}

bool initialize_file_faker_client(PID pid, ProcessInjectData* data)
{
	for (size_t i = 0; i < injected_processes_count; ++i)
	{
		if (injected_processes[i].pid == pid)
		{
			*data = injected_processes[i];
			return true;
		}
	}

	bool success = load_injector_library(pid);
	if (!success)
	{
		printf("Failed to inject to the process.");
		return false;
	}

	ProcessInjectData* old_datas = injected_processes;
	injected_processes_count++;
	injected_processes = malloc(sizeof(ProcessInjectData) * injected_processes_count);
	if (injected_processes == NULL)
	{
		return false;
	}

	if (old_datas != NULL)
	{
		memcpy(injected_processes, old_datas, sizeof(ProcessInjectData) * (injected_processes_count - 1));
		free(old_datas);
	}

	ProcessInjectData inject_data;
	inject_data.pid = pid;
	initialize_pipes(&inject_data);
	injected_processes[injected_processes_count - 1] = inject_data;
	*data = inject_data;
	return true;
}

bool get_named_pipe_name(PID pid, LPCSTR pipe_name, size_t size, bool serverWrite)
{
	char* local_pipe_prefix = "\\\\.\\pipe\\";
	strcpy(pipe_name, local_pipe_prefix);

	HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (process_handle == NULL)
	{
		printf("Failed to open process handle.");
		return false;
	}

	TCHAR exe_path[BUFFER_SIZE];
	BOOL success = GetModuleFileNameEx(process_handle, NULL, exe_path, sizeof(exe_path));
	if (!success)
	{
		printf("Failed to get process file name.");
		return false;
	}
	CloseHandle(process_handle);

	char* name_start = strrchr(exe_path, '\\') + 1;
	char* name_end = strchr(name_start, '.');
	*name_end = NULL;

	//~
	if (strlen(name_start) > 50)
	{
		printf("Too long process name.");
		return false;
	}

	strcat(pipe_name, name_start);
	char* pid_buffer[17];
	itoa(pid, pid_buffer, 10);
	strcat(pipe_name, "_");
	strcat(pipe_name, pid_buffer);
	strcat(pipe_name, "_");

	if (serverWrite)
	{
		strcat(pipe_name, "ServerWrite");
	}
	else
	{
		strcat(pipe_name, "ClientWrite");
	}

	return true;
}

void initialize_pipes(ProcessInjectData* data)
{
	get_named_pipe_name(data->pid, data->pipe_server_write_name, PIPE_NAME_SIZE, true);
	HANDLE pipe = CreateNamedPipe(data->pipe_server_write_name, PIPE_ACCESS_OUTBOUND, 0, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	if (pipe == NULL)
	{
		return;
	}
	data->pipe_server_write_handle = pipe;

	get_named_pipe_name(data->pid, data->pipe_server_read_name, PIPE_NAME_SIZE, false);
	pipe = CreateNamedPipe(data->pipe_server_read_name, PIPE_ACCESS_OUTBOUND, 0, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	if (pipe == NULL)
	{
		return;
	}
	data->pipe_server_read_handle = pipe;
}

REDIRECTION_HANDLE send_redirection_to(ProcessInjectData* data, char* file_path_to)
{
	ServerMessageData server_message_data;
	server_message_data.file_from_defined = false;
	strcpy(server_message_data.file_to, file_path_to);
	REDIRECTION_HANDLE handle = send_redirection(data, &server_message_data);
	return handle;
}

REDIRECTION_HANDLE send_redirection_from_to(ProcessInjectData* data, const char* file_path_from, const char* file_path_to)
{
	ServerMessageData server_message_data;
	server_message_data.file_from_defined = true;
	strcpy(server_message_data.file_from, file_path_from);
	strcpy(server_message_data.file_to, file_path_to);
	REDIRECTION_HANDLE handle = send_redirection(data, &server_message_data);
	return handle;
}

REDIRECTION_HANDLE send_redirection(ProcessInjectData* data, ServerMessageData* server_message_data)
{
	DWORD bytes_written;
	BOOL result = WriteFile(data->pipe_server_write_handle, &server_message_data, sizeof(ServerMessageData), &bytes_written, NULL);
	if (bytes_written != sizeof(ServerMessageData))
	{
		return INVALID_REDIRECTION_HANDLE;
	}

	ClientMessageData client_message_data;
	DWORD bytes_read;
	DWORD bytes_read_total = 0;
	while (true)
	{
		LPVOID buffer_start = &client_message_data + bytes_read_total;
		DWORD bytes_to_read = bytes_read_total - sizeof(ClientMessageData);
		result = ReadFile(data->pipe_server_read_handle, buffer_start, bytes_to_read, &bytes_read, NULL);
		if (!result)
		{
			return INVALID_REDIRECTION_HANDLE;
		}
		bytes_read_total += bytes_read;
		if (bytes_read_total == sizeof(ClientMessageData))
		{
			break;
		}
	}

	if (client_message_data.success)
	{
		RedirectionData* old_datas = redirection_datas;
		redirection_datas_count++;
		redirection_datas = malloc(sizeof(RedirectionData) * redirection_datas_count);
		if (redirection_datas == NULL)
		{
			return INVALID_REDIRECTION_HANDLE;
		}
		if (old_datas != NULL)
		{
			memcpy(redirection_datas, old_datas, sizeof(RedirectionData) * (redirection_datas_count - 1));
			free(old_datas);
		}

		RedirectionData redirection_data;
		redirection_data.pid = data->pid;
		REDIRECTION_HANDLE handle = ++redirection_counter;
		redirection_data.handle = handle;
		redirection_datas[redirection_datas_count - 1] = redirection_data;

		return handle;
	}
	else
	{
		printf("Failed to redirect file.");
		return INVALID_REDIRECTION_HANDLE;
	}
}