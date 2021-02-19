#include <cstdlib>
#include <string>
#include <iostream>
#include <algorithm>
#include <Windows.h>

#include "CommandType.h"

using process_runner_tests::CommandType;

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		std::cout << "Empty arguments list." << std::endl;
		return EXIT_SUCCESS;
	}

	CommandType command_type = static_cast<CommandType>(atoi(argv[1]));

	switch (command_type)
	{
	case CommandType::EmptyOutput:
	{
		return EXIT_SUCCESS;
	}
	case CommandType::Print5Lines:
	{
		size_t lines_count = 5;
		for (size_t i = 0; i < lines_count; ++i)
		{
			std::cout << "This is line #" << (i + 1) << std::endl;
		}
		break;
	}
	case CommandType::Wait3SecondsAndExit:
	{
		DWORD milliseconds = 3000;
		Sleep(milliseconds);
		return EXIT_SUCCESS;
	}
	case CommandType::Read3Lines:
	{
		std::string s;
		std::cin >> s;
		std::cin >> s;
		std::cin >> s;
		break;
	}
	case CommandType::ReadAndWriteReverse:
	{
		int input_size;
		std::cin >> input_size;

		for (size_t i = 0; i < input_size; ++i)
		{
			std::string s;
			std::cin >> s;

			std::reverse(s.begin(), s.end());
			std::cout << s << std::endl;
		}
		break;
	}
	case CommandType::PrintArguments:
	{
		for (size_t i = 1; i < argc; ++i)
		{
			std::cout << argv[i] << std::endl;
		}
		break;
	}
	case CommandType::ReadWrite3Lines:
	{
		std::string s;
		for (size_t i = 0; i < 3; ++i)
		{
			std::cin >> s;
			std::cout << "This is line #" << (i + 1) << std::endl;
		}
		break;
	}
	case CommandType::ExitCode111:
	{
		return 111;
	}
	case CommandType::ExitCode1234:
	{
		return 1234;
	}
	case CommandType::None:
	default:
	{
		std::cout << "Unrecognized command type.";
		break;
	}
	}

	return EXIT_SUCCESS;
}