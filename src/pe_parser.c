#include <stdbool.h>
#include <stdio.h>
#include <Windows.h>
#include <winnt.h>
#include <stdio.h>

#include "pe_parser.h"

bool assert(BYTE* data_to_check, BYTE* reference_data, size_t n, char* error_message)
{
	for (size_t i = 0; i < n; ++i)
	{
		if (data_to_check[i] != reference_data[i])
		{
			printf(error_message);
			return false;
		}
	}
	return true;
}

LPVOID find_function_address(const char* library_name, const char* function_name)
{
	HANDLE base_process_address = GetModuleHandle(NULL);
	if (base_process_address == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get process base address, error code = %d", error);
	}

	PIMAGE_DOS_HEADER dos_header = base_process_address;

	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf("Wrong MZ signature data.");
		return NULL;
	}

	PVOID pe_signature = (BYTE*)dos_header + dos_header->e_lfanew;

	if (!assert(pe_signature, IMAGE_NT_SIGNATURE, 4, "Wrong PE signature data."))
	{
		return NULL;
	}

	PIMAGE_FILE_HEADER image_header = (BYTE*)pe_signature + sizeof(IMAGE_NT_SIGNATURE);



	return NULL;
}