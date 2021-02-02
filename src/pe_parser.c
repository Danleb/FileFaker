#include <Windows.h>
#include <stdio.h>

#include "pe_parser.h"

const char pe_magic_header[] = { 'P', 'E', '\0', '\0' };

LPVOID find_function_address(const char* library_name, const char* function_name)
{
	HANDLE base_process_address = GetModuleHandle(NULL);
	if (base_process_address == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get process base address, error code = %d", error);
	}

	//byte* pe_start = module;
	//size_t offset_to_offset_to_pe_signature = 0x3c;
	//uint32_t offset_to_pe_signature = *(uint32_t*)(pe_start);
	//byte* pe_signature_address = pe_start + offset_to_pe_signature;
	////assert

	return NULL;
}