#include <stdbool.h>
#include <stdio.h>
#include <Windows.h>

#include "file_faker_server.h"

const char INJECT_LIBRARY_NAME[] = "FileFaker.dll";
const char KERNEL32_LIBRARY_NAME[] = "kernel32.dll";

#define LIBRARY_PATH_BUFFER_SIZE 1024
const char full_library_path[LIBRARY_PATH_BUFFER_SIZE];
LPCSTR load_library_function_name = TEXT("LoadLibrary");

bool load_injector_library(PID pid)
{
	DWORD desired_access = PROCESS_VM_READ |
		PROCESS_VM_WRITE |
		PROCESS_VM_OPERATION |
		PROCESS_CREATE_THREAD |
		PROCESS_QUERY_INFORMATION;
	HANDLE process_handle = OpenProcess(desired_access, FALSE, pid);
	if (process_handle == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to open process, error code = %d", error);
		return false;
	}

	DWORD full_path_length = GetFullPathName(INJECT_LIBRARY_NAME, LIBRARY_PATH_BUFFER_SIZE, full_library_path, NULL);
	if (full_path_length == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get full path, error code = %d", error);
		return false;
	}

	SIZE_T memory_size = full_path_length + 1;
	LPVOID allocated_memory = VirtualAllocEx(process_handle, NULL, memory_size, 999, PAGE_READWRITE);
	if (allocated_memory == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to allocate virtual memory, error code = %d", error);
		return false;
	}

	SIZE_T bytes_written;
	BOOL success = WriteProcessMemory(process_handle, allocated_memory, full_library_path, memory_size, &bytes_written);
	if (!success)
	{
		DWORD error = GetLastError();
		printf("Failed to write to process memory, error code = %d", error);
		return false;
	}

	HMODULE kernel_module_handle = GetModuleHandle(KERNEL32_LIBRARY_NAME);
	if (kernel_module_handle == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get kernel32.dll module handle, error code = %d", error);
		return false;
	}

	FARPROC load_library_function_address = GetProcAddress(kernel_module_handle, load_library_function_name);
	if (load_library_function_address == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get LoadLibrary function address, error code = %d", error);
		return false;
	}

	HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, load_library_function_address, allocated_memory, 0, NULL);
	if (thread_handle == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to create remote thread, error code = %d", error);
		return false;
	}

	return true;
}