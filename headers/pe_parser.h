#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

LPVOID find_function_address(const char* library_name, const char* function_name);

#ifdef __cplusplus
}
#endif