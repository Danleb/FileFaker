#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "file_faker_client.h"
#include "file_faker_server.h"

DWORD file_faker_client_loop(LPVOID lpThreadParameter)
{
	FILE* file = fopen("D:\\1.txt", "w");
	fprintf(file, "THIS IS OUTPUT FROM LOADED LIBRARY");
	fclose(file);

	//ProcessInjectData process_inject_data;
	//process_inject_data.pid = GetCurrentProcessId();
	//get_named_pipe_name(process_inject_data.pid, process_inject_data..pipe_server_write_handle, PIPE_NAME_SIZE, true);


	//ClientMessageData client_message_data;

	//while (true)
	//{
	//	/*command = wait_for_command();

	//	if (command.type == remove_redirection)
	//	{

	//	}
	//	else if (command.type == add_redirection)
	//	{

	//	}*/
	//}
	
	return EXIT_SUCCESS;
}