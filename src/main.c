#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "file_faker_server.h"

#define CMD_BUFFER_SIZE 2048

void print_error()
{
	printf("Invalid command. Type 'help' for help.\n");
}

void print_redirection_result(REDIRECTION_HANDLE redirection_handle)
{
	if (redirection_handle == INVALID_REDIRECTION_HANDLE)
	{
		printf("\nFailed to create redirection.\n");
	}
	else
	{
		printf("\nFile input was successfully redirected. Redirection Id = %d.\n", redirection_handle);
	}
}

int main()
{
	char file_from_buffer[CMD_BUFFER_SIZE];
	char file_to_buffer[CMD_BUFFER_SIZE];
	char command_buffer[CMD_BUFFER_SIZE];
	int pid;

	while (true)
	{
		int count = scanf_s("%2047s", command_buffer, CMD_BUFFER_SIZE);
		command_buffer[CMD_BUFFER_SIZE - 1] = 0;
		if (count != 1)
		{
			print_error();
		}
		else if (strcmp(command_buffer, "redirect-file") == 0 || strcmp(command_buffer, "rf") == 0)
		{
			gets_s(command_buffer, CMD_BUFFER_SIZE);
			command_buffer[CMD_BUFFER_SIZE - 1] = 0;
			pid = atoi(command_buffer);
			if (pid == 0)
			{
				print_error();
				continue;
			}

			printf("Print file from:\n");
			gets_s(file_from_buffer, CMD_BUFFER_SIZE);
			printf("Print file to:\n");
			gets_s(file_to_buffer, CMD_BUFFER_SIZE);

			REDIRECTION_HANDLE redirection_handle = redirect_file_io(pid, file_from_buffer, file_to_buffer);
			print_redirection_result(redirection_handle);
		}
		else if (strcmp(command_buffer, "redirect-files") == 0 || strcmp(command_buffer, "rfs") == 0)
		{
			gets_s(command_buffer, CMD_BUFFER_SIZE);
			command_buffer[CMD_BUFFER_SIZE - 1] = 0;
			pid = atoi(command_buffer);
			if (pid == 0)
			{
				print_error();
				continue;
			}

			gets_s(file_from_buffer, CMD_BUFFER_SIZE);
			printf("Print file to:");
			gets_s(file_from_buffer, CMD_BUFFER_SIZE);

			REDIRECTION_HANDLE redirection_handle = redirect_files_io(pid, file_from_buffer);
			print_redirection_result(redirection_handle);
		}
		else if (strcmp(command_buffer, "remove") == 0 || strcmp(command_buffer, "rm") == 0)
		{
			printf("Print redirection handle ID to remove:");
			int redirection_handle;
			int count = scanf("%s", &redirection_handle);
			if (count != 0)
			{
				print_error();
			}

			bool success = restore_file_io(pid, redirection_handle);
			if (success)
			{
				printf("Redirection was successfully removed.");
			}
			else
			{
				printf("Failed to remove redirection.");
			}
		}
		else if (strcmp(command_buffer, "help") == 0)
		{
			printf("To redirect one file input:\n" \
				"redirect-file|rf <process_id> <file_path_from> <file_path_to>\n" \
				"To redirect all file inputs to one file:\n" \
				"redirect-files|rfs <process_id> <file_path_to>\n"
				"To remove redirection by id:\n" \
				"remove|rm <redirection_id>\n"
			);
		}
		else
		{
			print_error();
		}
	}

	return EXIT_SUCCESS;
}