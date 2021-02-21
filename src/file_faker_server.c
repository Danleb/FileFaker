#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>

#include "injector_loader.h"
#include "file_faker_server.h"

#define BUFFER_SIZE 8192

const char CLIENT_READY_MESSAGE[] = "Client is ready.";

size_t redirection_datas_count = 0;
RedirectionData* redirection_datas = NULL;
REDIRECTION_HANDLE redirection_counter = 0;

size_t injected_processes_count = 0;
MessagingData* injected_processes = NULL;

bool initialize_file_faker_client(PID pid, MessagingData* data);
bool initialize_pipes(MessagingData* data);
bool wait_client_connection_is_ready(const MessagingData* data);

REDIRECTION_HANDLE send_redirection_to(const MessagingData* data, const char* file_path_to);
REDIRECTION_HANDLE send_redirection_from_to(const MessagingData* data, const char* file_path_from, const char* file_path_to);
REDIRECTION_HANDLE send_redirection(const MessagingData* data, const ServerMessageData* server_message_data);

REDIRECTION_HANDLE redirect_files_io(PID pid, const char* file_path_to)
{
	MessagingData* process_inject_data = NULL;
	if (!initialize_file_faker_client(pid, process_inject_data))
	{
		printf("Failed to inject to the process.\n");
		return INVALID_REDIRECTION_HANDLE;
	}
	REDIRECTION_HANDLE redirection_handle = send_redirection_to(pid, file_path_to);
	return redirection_handle;
}

REDIRECTION_HANDLE redirect_file_io(PID pid, const char* file_path_from, const char* file_path_to)
{
	MessagingData process_inject_data;
	if (!initialize_file_faker_client(pid, &process_inject_data))
	{
		printf("Failed to inject to the process.\n");
		return INVALID_REDIRECTION_HANDLE;
	}
	REDIRECTION_HANDLE redirection_handle = send_redirection_from_to(&process_inject_data, file_path_from, file_path_to);
	return redirection_handle;
}

bool initialize_file_faker_client(PID pid, MessagingData* data)
{
	for (size_t i = 0; i < injected_processes_count; ++i)
	{
		if (injected_processes[i].pid == pid)
		{
			*data = injected_processes[i];
			return true;
		}
	}

	MessagingData messaging_data;
	messaging_data.pid = pid;
	if (!initialize_pipes(&messaging_data))
	{
		printf("Failed to initialize pipes.\n");
		return false;
	}
	bool success = load_injector_library(pid);
	if (!success)
	{
		printf("Failed to inject to the process.\n");
		return false;
	}
	if (!wait_client_connection_is_ready(&messaging_data))
	{
		printf("Failed to wait client response.\n");
		return false;
	}

	MessagingData* old_datas = injected_processes;
	injected_processes_count++;
	injected_processes = malloc(sizeof(MessagingData) * injected_processes_count);
	if (injected_processes == NULL)
	{
		return false;
	}

	if (old_datas != NULL)
	{
		memcpy(injected_processes, old_datas, sizeof(MessagingData) * (injected_processes_count - 1));
		free(old_datas);
	}

	injected_processes[injected_processes_count - 1] = messaging_data;
	*data = messaging_data;
	return true;
}

bool get_named_pipe_name(PID pid, LPCSTR pipe_name, size_t size, bool serverWrite)
{
	char* local_pipe_prefix = "\\\\.\\pipe\\";
	strcpy(pipe_name, local_pipe_prefix);

	HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (process_handle == NULL)
	{
		printf("Failed to open process handle.\n");
		return false;
	}

	TCHAR exe_path[BUFFER_SIZE];
	BOOL success = GetModuleFileNameEx(process_handle, NULL, exe_path, sizeof(exe_path));
	if (!success)
	{
		printf("Failed to get process file name.\n");
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

bool initialize_pipes(MessagingData* data)
{
	get_named_pipe_name(data->pid, data->pipe_server_write_name, PIPE_NAME_SIZE, true);
	DWORD max_instances = 10;
	HANDLE pipe = CreateNamedPipe(data->pipe_server_write_name, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, max_instances, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	data->pipe_write_handle = pipe;

	get_named_pipe_name(data->pid, data->pipe_server_read_name, PIPE_NAME_SIZE, false);
	pipe = CreateNamedPipe(data->pipe_server_read_name, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE, max_instances, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	data->pipe_read_handle = pipe;
	return true;
}

bool wait_client_connection_is_ready(MessagingData* data)
{
	BOOL success = ConnectNamedPipe(data->pipe_read_handle, NULL);
	if (!success)
	{
		DWORD error_code = GetLastError();
		printf("Server failed to connect to the named pipe, error code = %d \n", error_code);
		return false;
	}

	success = ConnectNamedPipe(data->pipe_write_handle, NULL);
	if (!success)
	{
		DWORD error_code = GetLastError();
		if (error_code != ERROR_PIPE_CONNECTED)
		{
			printf("Server failed to connect to the named pipe, error code = %d \n", error_code);
			return false;
		}
	}

	char message_buffer[sizeof(CLIENT_READY_MESSAGE)];
	int bytes_required = strlen(CLIENT_READY_MESSAGE);
	DWORD bytes_read;
	DWORD total_bytes_read = 0;
	while (true)
	{
		LPVOID buffer_start = message_buffer + total_bytes_read;
		DWORD bytes_to_read = bytes_required - total_bytes_read;
		success = ReadFile(data->pipe_read_handle, buffer_start, bytes_to_read, &bytes_read, NULL);
		if (!success)
		{
			DWORD error_code = GetLastError();
			printf("Failed to read from named pipe, error code = %d \n", error_code);
			return false;
		}
		total_bytes_read += bytes_read;
		if (total_bytes_read == bytes_required)
		{
			break;
		}
	}

	message_buffer[sizeof(CLIENT_READY_MESSAGE) - 1] = 0;
	if (strcmp(message_buffer, CLIENT_READY_MESSAGE) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

REDIRECTION_HANDLE send_redirection_to(MessagingData* data, char* file_path_to)
{
	ServerMessageData server_message_data;
	server_message_data.command_type = AddRedirection;
	server_message_data.redirection_data.file_from_defined = false;
	strcpy(server_message_data.redirection_data.file_path_to, file_path_to);
	REDIRECTION_HANDLE handle = send_redirection(data, &server_message_data);
	return handle;
}

REDIRECTION_HANDLE send_redirection_from_to(MessagingData* data, const char* file_path_from, const char* file_path_to)
{
	ServerMessageData server_message_data;
	server_message_data.command_type = AddRedirection;
	server_message_data.redirection_data.file_from_defined = true;
	strcpy(server_message_data.redirection_data.file_path_from, file_path_from);
	strcpy(server_message_data.redirection_data.file_path_to, file_path_to);
	REDIRECTION_HANDLE handle = send_redirection(data, &server_message_data);
	return handle;
}

REDIRECTION_HANDLE send_redirection(MessagingData* data, ServerMessageData* server_message_data)
{
	DWORD bytes_written;
	BOOL result = WriteFile(data->pipe_write_handle, server_message_data, sizeof(ServerMessageData), &bytes_written, NULL);
	if (bytes_written != sizeof(ServerMessageData))
	{
		return INVALID_REDIRECTION_HANDLE;
	}

	ClientMessageData client_message_data;
	DWORD bytes_read;
	DWORD bytes_read_total = 0;
	while (true)
	{
		LPVOID buffer_start = (char*)&client_message_data + bytes_read_total;
		DWORD bytes_to_read = sizeof(ClientMessageData) - bytes_read_total;
		result = ReadFile(data->pipe_read_handle, buffer_start, bytes_to_read, &bytes_read, NULL);
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