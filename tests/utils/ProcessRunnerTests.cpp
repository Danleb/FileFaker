#include <gtest/gtest.h>
#include <Windows.h>

#include "ProcessRunner.h"

namespace process_runner_tests
{
	const char* TEST_PROGRAM_PATH = "TestFileReaderProgram.exe";

	TEST(ProcessRunnerTests, ProcessRunnerTest)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		const char* expected = "Empty input";
		std::string output = runner.Run();
		ASSERT_EQ(output, expected);
	}

	TEST(ProcessRunnerTests, ProcessRunnerTestWithInput)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		const char* input = "Input from parent process";
		std::string output = runner.Run(input);
		ASSERT_EQ(output, input);
	}
}