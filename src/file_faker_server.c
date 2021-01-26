#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "injector_loader.h"
#include "file_faker.h"

RedirectionData* redirection_datas = NULL;
size_t redirection_datas_count = 0;
REDIRECTION_HANDLE redirection_counter = 0;

REDIRECTION_HANDLE redirect_file_io(PID pid, char* file_path_to)
{	
	initialize(pid);
	send_redirection(pid, file_path_to);
	REDIRECTION_HANDLE handle = ++redirection_counter;
	return handle;
}

void initialize(PID pid)
{
	for (size_t i = 0; i < injected_processes_count; ++i)
	{
		if (injected_processes[i] == pid)
		{
			return;
		}
	}

	bool success = load_injector_library(pid);
	if (success)
	{
		RedirectionData* old_datas = redirection_datas;
		redirection_datas_count++;
		redirection_datas = malloc(sizeof(RedirectionData) * redirection_datas_count];
		if (old_datas != NULL)
		{
			memcpy(redirection_datas, old_datas, sizeof(RedirectionData) * redirection_datas_count);
			free(old_datas);
		}		
	}
	else
	{
		printf("Failed to inject to the process.");
	}
}