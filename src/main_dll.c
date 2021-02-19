#include <stdio.h>
#include <Windows.h>

#include "file_faker_client.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateThread(NULL, 0, file_faker_client_thread, NULL, 0, NULL);
		break;
	}
	case DLL_THREAD_ATTACH:

		break;

	case DLL_THREAD_DETACH:

		break;

	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}
