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
	};

	TEST_F(FileFakerTestsChildProcessF, fopen_Test)
	{
		utils::ProcessRunner runner(TestProgram);
		auto argument = std::to_string(static_cast<int>(CommandType::fopen));
		runner.Run(argument);
		Sleep(100);
		runner.WriteLine(TrueInfoFileName);
		std::string text_from_file = runner.ReadToEnd();
		runner.WaitForExit();
		ASSERT_EQ(runner.GetExitCode(), EXIT_SUCCESS);
		ASSERT_EQ(FakedInfo, text_from_file);
	}
}