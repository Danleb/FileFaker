#include <fstream>
#include <gtest/gtest.h>
#include <Windows.h>

#include "pe_parser.h"

namespace file_faker_tests
{
	TEST(PE_ParserTests, FindFunction_GetCurrentProcessId)
	{
		DWORD pid = GetCurrentProcessId();
		LPVOID address = find_function_address("GetCurrentProcessId", "Kernel32.dll");
		ASSERT_NE(address, nullptr);
	}
}