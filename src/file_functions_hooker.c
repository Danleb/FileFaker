#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>

#include "imports_finder.h"
#include "hook_file_functions.h"
#include "file_functions_hooker.h"

#define MAX_IMPORT_ADDRESSES 200

bool hook_file_function(void* original_function, void* hook_function);

bool hook_file_functions()
{
	void* functions_to_hook[] = {
		fopen,
		CreateFileA,
		CreateFileW,
	};
	void* hook_functions[] = {
		fopen_fake,
		CreateFileA_fake,
		CreateFileW_fake,
	};
	void** original_addresses[] = {
		&fopen_Original,
		&CreateFileA_Original,
		&CreateFileW_Original,
	};

	size_t count = sizeof(functions_to_hook) / sizeof(void*);
	size_t count2 = sizeof(hook_functions) / sizeof(void*);
	size_t count3 = sizeof(original_addresses) / sizeof(void*);
	if (count != count2 || count != count3)
	{
		printf("Functions arrays have different sizes.\n");
		return false;
	}

	for (size_t i = 0; i < count; ++i)
	{
		*original_addresses[i] = functions_to_hook[i];
		bool success = hook_file_function(functions_to_hook[i], hook_functions[i]);
		if (!success)
		{
			printf("Failed to hook function #%llu", i + 1);
			return false;
		}
	}
}

bool hook_file_function(void* original_function, void* hook_function)
{
	void* addresses[MAX_IMPORT_ADDRESSES];
	memset(addresses, 0, MAX_IMPORT_ADDRESSES * sizeof(void*));
	int count = find_imported_function_addresses(original_function, addresses, MAX_IMPORT_ADDRESSES);
	if (count == 0)
	{
		return true;
	}
	if (count == -1)
	{
		return false;
	}

	DWORD pid = GetCurrentProcessId();
	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (process_handle == NULL)
	{
		return false;
	}

	size_t pointer_size = sizeof(void*);

	for (size_t i = 0; i < count; ++i)
	{
		void* address = addresses[i];

		DWORD old_protect;
		DWORD new_protect = PAGE_EXECUTE_READWRITE;
		BOOL success = VirtualProtect(address, pointer_size, new_protect, &old_protect);
		if (!success)
		{
			DWORD error_code = GetLastError();
			printf("Failed to VirtualProtect, error code = %d\n", error_code);
			return false;
		}

		SIZE_T bytes_written;
		success = WriteProcessMemory(process_handle, address, &hook_function, pointer_size, &bytes_written);

		if (!success)
		{
			DWORD error_code = GetLastError();
			printf("Failed to WriteProcessMemory, error code = %d\n", error_code);
			return false;
		}
		if (bytes_written != pointer_size)
		{
			printf("Failed to write %llu bytes.\n", pointer_size);
			return false;
		}

		new_protect = old_protect;
		success = VirtualProtect(address, pointer_size, new_protect, &old_protect);
		if (!success)
		{
			DWORD error_code = GetLastError();
			printf("Failed to restore VirtualProtect, error code = %d\n", error_code);
			return false;
		}
	}

	CloseHandle(process_handle);
	return true;
}