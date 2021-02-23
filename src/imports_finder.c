#include <stdbool.h>
#include <stdio.h>
#include <Windows.h>
#include <winnt.h>
#include <stdio.h>
#include <Psapi.h>

#include "imports_finder.h"

//#define PRINT_IMPORTS

LPVOID find_imported_function_address(void* required_imported_function, HMODULE module);

int find_imported_function_addresses(void* imported_function_address, void** addresses_list, int addresses_count)
{
	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
	if (process_handle == INVALID_HANDLE_VALUE)
	{
		DWORD error_code = GetLastError();
		printf("Failed to open process, error code = %d", error_code);
		return -1;
	}

	DWORD bytes_needed;
	if (!EnumProcessModules(process_handle, NULL, 0, &bytes_needed))
	{
		DWORD error_code = GetLastError();
		printf("Failed to enum process modules, error code = %d", error_code);
		return -1;
	}

	size_t loaded_modules_count = bytes_needed / sizeof(HMODULE);
	DWORD required_size = bytes_needed;
	HMODULE* modules_list = malloc(required_size);
	if (!EnumProcessModules(process_handle, modules_list, required_size, &bytes_needed))
	{
		DWORD error_code = GetLastError();
		printf("Failed to enum process modules, error code = %d", error_code);
		return -1;
	}

	size_t found_count = 0;
	for (size_t i = 0; i < loaded_modules_count; ++i)
	{
		HMODULE module = modules_list[i];

#ifdef PRINT_IMPORTS
		TCHAR module_name[MAX_PATH];
		if (GetModuleFileNameEx(process_handle, module, module_name, sizeof(module_name) / sizeof(TCHAR)))
		{
			printf(TEXT("Module name: %s (%p)\n"), module_name, module);
		}
#endif

		LPVOID function_address = find_imported_function_address(imported_function_address, module);
		if (function_address != NULL)
		{
			addresses_list[found_count] = function_address;
			found_count++;
		}

		if (found_count >= addresses_count)
		{
			printf("Found too many function imports.");
			break;
		}
	}

	free(modules_list);
	CloseHandle(process_handle);
	return found_count;
}

LPVOID find_imported_function_address(void* required_imported_function, HMODULE module)
{
	HMODULE base_module_address = module;

	PIMAGE_DOS_HEADER dos_header = base_module_address;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf("Wrong MZ signature data.");
		return NULL;
	}

	PWORD pe_signature = (BYTE*)dos_header + dos_header->e_lfanew;
	if (*pe_signature != IMAGE_NT_SIGNATURE)
	{
		printf("Wrong PE signature data.\n");
		return NULL;
	}

	PIMAGE_FILE_HEADER image_header = (BYTE*)pe_signature + sizeof(IMAGE_NT_SIGNATURE);
	PIMAGE_OPTIONAL_HEADER32 optional_header = (BYTE*)image_header + sizeof(IMAGE_FILE_HEADER);

	bool is32bit;
	PIMAGE_SECTION_HEADER first_section_header;
	PIMAGE_DATA_DIRECTORY image_data_directories;

	if (optional_header->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
	{
		is32bit = true;
		first_section_header = (BYTE*)optional_header + sizeof(IMAGE_OPTIONAL_HEADER32);
		image_data_directories = &optional_header->DataDirectory;
	}
	else if (optional_header->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		is32bit = false;
		PIMAGE_OPTIONAL_HEADER64 optional_header64 = optional_header;
		optional_header = NULL;
		first_section_header = (BYTE*)optional_header64 + sizeof(IMAGE_OPTIONAL_HEADER64);
		image_data_directories = &optional_header64->DataDirectory;
	}
	else
	{
		printf("Unrecognized architecture bit.");
		return NULL;
	}

	IMAGE_DATA_DIRECTORY import_image_data_directory = image_data_directories[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (import_image_data_directory.VirtualAddress == NULL)
	{
		return NULL;
	}
	PIMAGE_IMPORT_DESCRIPTOR import_descriptor = (BYTE*)base_module_address + import_image_data_directory.VirtualAddress;
	while (import_descriptor->Characteristics != NULL)
	{
#ifdef PRINT_IMPORTS
		PCHAR module_name = (BYTE*)base_module_address + import_descriptor->Name;
		printf("\nLibrary name: %s \n", module_name);
#endif

		if (is32bit)
		{
			return NULL;
		}
		else
		{
			PIMAGE_THUNK_DATA64 hint_name_array = (BYTE*)base_module_address + import_descriptor->OriginalFirstThunk;
			PIMAGE_THUNK_DATA64 iat_array = (BYTE*)base_module_address + import_descriptor->FirstThunk;

			for (;; hint_name_array++, iat_array++)
			{
				if (hint_name_array->u1.Ordinal == 0)
				{
					break;
				}

				if (hint_name_array->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				{
					continue;
				}

#ifdef PRINT_IMPORTS
				CHAR* function_name = (BYTE*)base_module_address + hint_name_array->u1.AddressOfData + 2;
				printf("%s \n", function_name);
#endif

				BYTE** function_address = &(iat_array->u1.Function);

				if (*function_address == required_imported_function)
				{
					return function_address;
				}
			}
		}

		import_descriptor++;
	}

	return NULL;
}