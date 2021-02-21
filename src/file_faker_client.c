#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "file_functions_hooker.h"
#include "redirections_manager.h"
#include "file_faker_client.h"
#include "file_faker_server.h"

bool file_faker_client_loop(const FILE* file);
bool wait_for_message(const MessagingData* process_inject_data, ServerMessageData* server_message_data);

DWORD file_faker_client_thread(LPVOID lpThreadParameter)
{
	FILE* file = fopen("D:\\1.txt", "w");
	fprintf(file, "THIS IS OUTPUT FROM LOADED LIBRARY");
	file_faker_client_loop(file);
	fclose(file);
	return EXIT_SUCCESS;
}

bool file_faker_client_loop(const FILE* file)
{
	MessagingData messaging_data;
	messaging_data.pid = GetCurrentProcessId();
	fprintf(file, "PID=%d", messaging_data.pid);

	if (!hook_file_functions())
	{
		printf("Failed to hook file functions.\n");
		return false;
	}

	bool success = get_named_pipe_name(messaging_data.pid, messaging_data.pipe_server_write_name, PIPE_NAME_SIZE, true);
	if (!success)
	{
		printf("Client failed to get pipe name.");
		return false;
	}

	success = get_named_pipe_name(messaging_data.pid, messaging_data.pipe_server_read_name, PIPE_NAME_SIZE, false);
	if (!success)
	{
		return false;
	}

	HANDLE client_write_end = CreateFile(messaging_data.pipe_server_read_name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (client_write_end == INVALID_HANDLE_VALUE)
	{
		DWORD error_code = GetLastError();
		return false;
	}
	messaging_data.pipe_write_handle = client_write_end;

	HANDLE client_read_end = CreateFile(messaging_data.pipe_server_write_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (client_read_end == INVALID_HANDLE_VALUE)
	{
		DWORD error_code = GetLastError();
		return false;
	}
	messaging_data.pipe_read_handle = client_read_end;

	DWORD bytes_written;
	DWORD message_size = strlen(CLIENT_READY_MESSAGE);
	BOOL written = WriteFile(client_write_end, CLIENT_READY_MESSAGE, message_size, &bytes_written, NULL);
	if (!written || bytes_written != message_size)
	{
		DWORD error_code = GetLastError();
		printf("Client ready was not sent, error code = %d", error_code);
		return false;
	}
	ClientMessageData client_message_data;
	ServerMessageData server_message_data;

	while (true)
	{
		if (!wait_for_message(&messaging_data, &server_message_data))
		{
			break;
		}

		switch (server_message_data.command_type)
		{
		case AddRedirection:
		{
			client_message_data.success = add_redirection(server_message_data.redirection_data);
			break;
		}
		case RemoveRedirection:
		{
			client_message_data.success = remove_redirection(server_message_data.redirection_data.handle);
			break;
		}
		}

		bytes_written;
		message_size = sizeof(ClientMessageData);
		written = WriteFile(client_write_end, &client_message_data, message_size, &bytes_written, NULL);
		if (!written || bytes_written != message_size)
		{
			printf("Client message was not sent.");
			return false;
		}
	}

	return true;
}

bool wait_for_message(const MessagingData* messaging_data, ServerMessageData* server_message_data)
{
	DWORD bytes_read;
	DWORD bytes_read_total = 0;
	while (true)
	{
		LPVOID buffer_start = (char*)server_message_data + bytes_read_total;
		DWORD bytes_to_read = sizeof(ServerMessageData) - bytes_read_total;
		BOOL success = ReadFile(messaging_data->pipe_read_handle, buffer_start, bytes_to_read, &bytes_read, NULL);
		if (!success)
		{
			return false;
		}
		bytes_read_total += bytes_read;
		if (bytes_read_total == sizeof(ServerMessageData))
		{
			break;
		}
	}

	return true;
}