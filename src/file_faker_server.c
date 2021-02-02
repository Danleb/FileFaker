#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "injector_loader.h"
#include "file_faker_server.h"

#define BUFFER_SIZE 1024

size_t redirection_datas_count = 0;
RedirectionData* redirection_datas = NULL;
REDIRECTION_HANDLE redirection_counter = 0;

size_t injected_processes_count = 0;
ProcessInjectData* injected_processes = NULL;

void initialize_file_faker_client(PID pid);
void initialize_pipes(ProcessInjectData* data);

REDIRECTION_HANDLE send_redirection_to(ProcessInjectData* data, char* file_path_to);
//REDIRECTION_HANDLE send_redirection_from_to(ProcessInjectData* data, char* file_path_from, char* file_path_to);

REDIRECTION_HANDLE redirect_file_io(PID pid, char* file_path_to)
{
	initialize_file_faker_client(pid);
	REDIRECTION_HANDLE redirection_handle = send_redirection_to(pid, file_path_to);
	return redirection_handle;
}

REDIRECTION_HANDLE redirect_files_io(PID pid, char* file_path_from)
{
	return -1;
}

void initialize_file_faker_client(PID pid)
{
	for (size_t i = 0; i < injected_processes_count; ++i)
	{
		if (injected_processes[i].pid == pid)
		{
			return;
		}
	}

	bool success = load_injector_library(pid);
	if (success)
	{
		ProcessInjectData* old_datas = injected_processes;
		injected_processes_count++;
		injected_processes = malloc(sizeof(ProcessInjectData) * injected_processes_count);
		if (injected_processes == NULL)
		{
			return;
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
	}
	else
	{
		printf("Failed to inject to the process.");
	}
}

bool get_named_pipe_name(PID pid, LPCSTR pipe_name, size_t size, bool serverWrite)
{
	//"\\\\.\\pipe\\"
	//GetNamedPipeClientComputerName

	//GetProcessInformation
	//HMODULE process_handle = GetModuleHandle(NULL);

	HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (process_handle == NULL)
	{
		printf("Failed to open process handle.");
		return false;
	}

	CHAR exe_name[BUFFER_SIZE];
	BOOL success = QueryFullProcessImageName(process_handle, 0, exe_name, BUFFER_SIZE);



	//LPWSTR module_file_name[BUFFER_SIZE];
	//DWORD result = GetModuleFileName(process_handle, module_file_name, BUFFER_SIZE);
	CloseHandle(process_handle);
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


}

REDIRECTION_HANDLE send_redirection_to(ProcessInjectData* data, char* file_path_to)
{
	ServerMessageData server_message_data;	
	server_message_data.file_from_defined = false;
	strcpy(server_message_data.file_to, file_path_to);

	LPDWORD bytes_written;
	BOOL result = WriteFile(data->pipe_server_write_handle, &server_message_data, sizeof(ServerMessageData), bytes_written, NULL);
	//assert

	ClientMessageData client_message_data;
	LPDWORD bytes_read;
	result = ReadFile(data->pipe_server_read_handle, &client_message_data, sizeof(client_message_data), bytes_read, NULL);

	if (client_message_data.success)
	{
		RedirectionData* old_datas = redirection_datas;
		redirection_datas_count++;
		redirection_datas = malloc(sizeof(RedirectionData) * redirection_datas_count);
		if (old_datas != NULL && redirection_datas != NULL)
		{
			memcpy(redirection_datas, old_datas, sizeof(RedirectionData) * (redirection_datas_count - 1));
			free(old_datas);
		}

		RedirectionData redirection_data;
		redirection_data.pid = data->pid;
		REDIRECTION_HANDLE handle = ++redirection_counter;
		redirection_data.handle = handle;
	}
	else
	{
		printf("Failed to redirect file.");
	}
}
