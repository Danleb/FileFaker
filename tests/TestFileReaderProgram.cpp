#include <iostream>
#include <cstdio>
#include <Windows.h>

#include "CommandType.h"

#define BUFFER_SIZE 2048

using file_faker_tests::CommandType;

int main()
{
	std::string file_path;
	std::cin >> file_path;

	int command;
	std::cin >> command;
	CommandType command_type = static_cast<CommandType>(command);

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	DWORD bytes_read;

	switch (command_type)
	{
	case CommandType::fopen:
	{
		FILE* file_handle = fopen(file_path.c_str(), "r");
		if (file_handle == NULL)
		{
			printf("Failed to open file by path: %s", file_path.c_str());
			return EXIT_FAILURE;
		}

		char c = fgetc(file_handle);
		while (c != EOF)
		{
			printf("%c", c);
			c = fgetc(file_handle);
		}
		fclose(file_handle);
		break;
	}
	case CommandType::CreateFileA:
	{
		HANDLE file = CreateFileA(file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		BOOL success = ReadFile(file, buffer, BUFFER_SIZE, &bytes_read, NULL);
		if (!success)
		{
			DWORD error_code = GetLastError();
			std::cout << "Failed to ReadFile, error code = " << error_code << std::endl;
			break;
		}
		CloseHandle(file);
		break;
	}
	case CommandType::CreateFileW:
	{
//std::wstring wtrue_info_file_path(true_info_file_path.begin(), true_info_file_path.end());
		/*HANDLE file = CreateFileW(file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		BOOL success = ReadFile(file, buffer, BUFFER_SIZE, &bytes_read, NULL);
		if (!success)
		{
			DWORD error_code = GetLastError();
			std::cout << "Failed to ReadFile, error code = " << error_code << std::endl;
			break;
		}
		CloseHandle(file);
		break;*/
	}
	case CommandType::ofstream:
	{

		break;
	}
	default:
	{
		std::cout << "Unrecognized CommandType: " << command << std::endl;
		break;
	}
	}

	return EXIT_SUCCESS;
}