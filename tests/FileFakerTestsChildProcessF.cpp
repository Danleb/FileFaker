#include <fstream>
#include <gtest/gtest.h>
#include <Windows.h>

#include "file_faker_server.h"
#include "CommandType.h"
#include "FileFakerTestsSameProcessF.h"
#include "ProcessRunner.h"

namespace file_faker_tests
{
	class FileFakerTestsChildProcessF : public FileFakerTestsSameProcessF
	{
	protected:
		void SetUp() override
		{
			CreateTestFiles(true_info_file_path, faked_info_file_path);
		}

		void ExecuteTest(CommandType command_type)
		{
			utils::ProcessRunner runner(TestProgram);
			runner.Run();
			Sleep(100);
			REDIRECTION_HANDLE redirection_handle = redirect_file_io(runner.GetProcessId(), true_info_file_path.data(), faked_info_file_path.data());
			ASSERT_EQ(redirection_handle, 1);
			auto command = std::to_string(static_cast<int>(CommandType::fopen));
			runner.WriteLine(command);
			runner.WriteLine(true_info_file_path.data());
			std::string text_from_file = runner.ReadToEnd();
			runner.WaitForExit();
			ASSERT_EQ(FakedInfo, text_from_file);
			ASSERT_EQ(runner.GetExitCode(), EXIT_SUCCESS);
		}
	};

	TEST_F(FileFakerTestsChildProcessF, fopen_Test)
	{
		ExecuteTest(CommandType::fopen);
	}

	TEST_F(FileFakerTestsChildProcessF, CreateFileA_Test)
	{
		ExecuteTest(CommandType::CreateFileA);
	}

	TEST_F(FileFakerTestsChildProcessF, CreateFileW_Test)
	{
		ExecuteTest(CommandType::CreateFileW);
	}

	TEST_F(FileFakerTestsChildProcessF, ifstream_Test)
	{
		ExecuteTest(CommandType::ifstream);
	}
}