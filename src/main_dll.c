#include <Windows.h>
#include "pe_parser.h"
#include "redirections_receiver.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		LPSECURITY_ATTRIBUTES security_attributes;
		CreateThread(security_attributes, 0, main_loop, NULL, 0, NULL);
		break;

	case DLL_THREAD_ATTACH:

		break;

	case DLL_THREAD_DETACH:

		break;

	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}

void main_loop()
{


	while (true)
	{
		/*command = wait_for_command();

		if (command.type == remove_redirection)
		{

		}
		else if (command.type == add_redirection)
		{

		}*/
	}
}