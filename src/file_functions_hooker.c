#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>

#include "imports_finder.h"
#include "hook_file_functions.h"
#include "file_functions_hooker.h"

#define MAX_IMPORT_ADDRESSES 500

typedef struct HookData {
	void* function_to_hook;
	void* hook_function;
	void** original_address;
} HookData;

bool hook_file_function(void* original_function, void* hook_function);

bool hook_file_functions()
{
	//todo _wfopen_s, freopen, _wfreopen,
	HookData hook_datas[] = {
		{
			.function_to_hook = fopen,
			.hook_function = fopen_fake,
			.original_address = &fopen_Original
		},
		{
			.function_to_hook = fopen_s,
			.hook_function = fopen_s_fake,
			.original_address = &fopen_s_Original
		},
		{
			.function_to_hook = _fsopen,
			.hook_function = _fsopen_fake,
			.original_address = &_fsopen_Original
		},
		{
			.function_to_hook = _wfsopen,
			.hook_function = _wfsopen_fake,
			.original_address = &_wfsopen_Original
		},
		{
			.function_to_hook = CreateFileA,
			.hook_function = CreateFileA_fake,
			.original_address = &CreateFileA_Original
		},
		{
			.function_to_hook = CreateFileW,
			.hook_function = CreateFileW_fake,
			.original_address = &CreateFileW_Original
		},
		{
			.function_to_hook = _wfopen,
			.hook_function = _wfopen_fake,
			.original_address = &_wfopen_Original
		},
	};
	size_t count = sizeof(hook_datas) / sizeof(HookData);
	for (size_t i = 0; i < count; ++i)
	{
		HookData* hook_data = &hook_datas[i];
		*(hook_data->original_address) = hook_data->function_to_hook;
		//*original_addresses[i] = functions_to_hook[i];
		bool success = hook_file_function(hook_data->function_to_hook, hook_data->hook_function);
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