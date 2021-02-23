#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "file_faker_server.h"

#define CMD_BUFFER_SIZE 2048

int main()
{
	char file_from_buffer[CMD_BUFFER_SIZE];
	char file_to_buffer[CMD_BUFFER_SIZE];
	char command_buffer[CMD_BUFFER_SIZE];
	int pid;

	while (true)
	{
		int result = scanf("%s", command_buffer);
		if (!result)
		{
			printf("Invalid command.\n");
			continue;
		}

		command_buffer[CMD_BUFFER_SIZE - 1] = 0;
		if (strcmp("redirect", command_buffer) == 0)
		{
			result = scanf("%d %s", &pid, &file_from_buffer);

			if (result < 2)
			{
				printf("Invalid arguments count.");
				continue;
			}

			char* input = fgets(file_to_buffer, CMD_BUFFER_SIZE, stdin);
			result = sscanf(file_to_buffer, "%s", file_to_buffer);

			int redirection_handle = -1;
			if (result)
			{
				redirection_handle = redirect_files_io(pid, file_from_buffer, file_to_buffer);
			}
			else
			{
				redirection_handle = redirect_files_io(pid, file_from_buffer);
			}

			if (redirection_handle == -1)
			{
				printf("Failed to create redirection.");
			}
			else
			{
				printf("File input was successfully redirected. Rediretion Id = %d", redirection_handle);
			}
		}
		else if (strcmp("restore", command_buffer) == 0)
		{
			/*result = scanf("%d", &pid);
			if (!result)
			{
				printf("Invalid process id input.");
				continue;
			}*/

			int redirection_handle;
			result = scanf("%d", &redirection_handle);

			if (!result)
			{
				//bool success = restore_file_io(redirection_handle);
				//if (success)
				//{

				//}
			}
		}
		else
		{
			printf("Invalid command\n");
		}
	}

	return EXIT_SUCCESS;
}