#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int find_imported_function_addresses(void* imported_function_address, void** addresses_list, int addresses_count);

#ifdef __cplusplus
}
#endif