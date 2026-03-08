#include <stdbool.h>
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>
#include <PathCch.h>

#include "injector_loader.h"

const WCHAR INJECT_LIBRARY_NAME[] = L"FileFakerClient.dll";
const char KERNEL32_LIBRARY_NAME[] = "kernel32.dll";
const char LOAD_LIBRARY_FUNCTION_NAME[] = "LoadLibraryW";

#define LIBRARY_PATH_BUFFER_SIZE 1024
WCHAR full_library_path[LIBRARY_PATH_BUFFER_SIZE];

bool load_injector_library(PID pid)
{
	DWORD desired_access = PROCESS_VM_READ |
		PROCESS_VM_WRITE |
		PROCESS_VM_OPERATION |
		PROCESS_CREATE_THREAD |
		PROCESS_QUERY_INFORMATION;

	HANDLE current_process = GetCurrentProcess();
	WCHAR exe_path[LIBRARY_PATH_BUFFER_SIZE];
	BOOL success = GetModuleFileNameExW(current_process, NULL, exe_path, LIBRARY_PATH_BUFFER_SIZE);
	if (!success)
	{
		printf("Failed to get process file name.\n");
		return false;
	}

	HRESULT res = PathCchRemoveFileSpec(exe_path, LIBRARY_PATH_BUFFER_SIZE);
	if (res != S_OK)
	{
		printf("Failed to get directory from exe path, error code = %d.", res);
		return false;
	}

	res = PathCchCombineEx(full_library_path, LIBRARY_PATH_BUFFER_SIZE, exe_path, INJECT_LIBRARY_NAME, PATHCCH_ALLOW_LONG_PATHS);
	if (res != S_OK)
	{
		printf("Failed to combine path, error code = %d.", res);
		return false;
	}

	SIZE_T full_path_length = wcsnlen_s(full_library_path, LIBRARY_PATH_BUFFER_SIZE);
	if (full_path_length == LIBRARY_PATH_BUFFER_SIZE)
	{
		printf("Full library path is too long.\n");
		return false;
	}

	DWORD file_attributes = GetFileAttributesW(full_library_path);
	bool file_exists = (file_attributes != INVALID_FILE_ATTRIBUTES) && !(file_attributes & FILE_ATTRIBUTE_DIRECTORY);
	if (!file_exists)
	{
		printf("Injector library does not exist in the expected path.\n");
		return false;
	}

	HANDLE process_handle = OpenProcess(desired_access, FALSE, pid);
	if (process_handle == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to open process, error code = %d.", error);
		return false;
	}

	SIZE_T memory_size = (full_path_length + 1) * sizeof(WCHAR);
	LPVOID allocated_memory = VirtualAllocEx(process_handle, NULL, memory_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (allocated_memory == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to allocate virtual memory, error code = %d.", error);
		CloseHandle(process_handle);
		return false;
	}

	SIZE_T bytes_written;
	success = WriteProcessMemory(process_handle, allocated_memory, full_library_path, memory_size, &bytes_written);
	if (bytes_written != memory_size)
	{
		printf("Failed to write full path to the process memory.");
		CloseHandle(process_handle);
		return false;
	}
	if (!success)
	{
		DWORD error = GetLastError();
		printf("Failed to write to process memory, error code = %d.", error);
		CloseHandle(process_handle);
		return false;
	}

	HMODULE kernel_module_handle = GetModuleHandle(KERNEL32_LIBRARY_NAME);
	if (kernel_module_handle == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get kernel32.dll module handle, error code = %d.", error);
		CloseHandle(process_handle);
		return false;
	}

	FARPROC load_library_function_address = GetProcAddress(kernel_module_handle, LOAD_LIBRARY_FUNCTION_NAME);
	if (load_library_function_address == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to get LoadLibrary function address, error code = %d.", error);
		CloseHandle(process_handle);
		return false;
	}

	HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, load_library_function_address, allocated_memory, 0, NULL);
	if (thread_handle == NULL)
	{
		DWORD error = GetLastError();
		printf("Failed to create remote thread, error code = %d.", error);
		CloseHandle(process_handle);
		return false;
	}

	CloseHandle(process_handle);
	return true;
}