#include <stdbool.h>
#include <Windows.h>

#include "file_faker_client.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//LPSECURITY_ATTRIBUTES security_attributes;
		//CreateThread(security_attributes, 0, file_faker_client_loop, NULL, 0, NULL);
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
