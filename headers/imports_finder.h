#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int find_imported_function_addresses(void* imported_function_address, void** addresses_list, int addresses_count);
//LPVOID find_function_address(const char* library_name, const char* function_name);

#ifdef __cplusplus
}
#endif