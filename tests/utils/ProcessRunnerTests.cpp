#include <ctime>
#include <gtest/gtest.h>
#include <Windows.h>

#include "ProcessRunner.h"
#include "CommandType.h"

namespace process_runner_tests
{
	const char* TEST_PROGRAM_PATH = "ProcessRunnerTestProgram.exe";

	TEST(ProcessRunnerTests, ProcessRunnerTest_Run)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		runner.Run();
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_RunWait)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		runner.Run();
		runner.WaitForExit();
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_Terminate)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		runner.Run();
		runner.Terminate();
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_RunAndWait)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		std::string output = runner.RunAndWait()[0];
		ASSERT_EQ(output, "Empty arguments list.");
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_EmptyOutput)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::EmptyOutput));
		auto lines = runner.RunAndWait(argument);
		ASSERT_TRUE(lines.empty());
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_PrintSeveralLines)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::Print5Lines));
		auto lines = runner.RunAndWait(argument);
		ASSERT_EQ(lines[0], "This is line #1");
		ASSERT_EQ(lines[1], "This is line #2");
		ASSERT_EQ(lines[2], "This is line #3");
		ASSERT_EQ(lines[3], "This is line #4");
		ASSERT_EQ(lines[4], "This is line #5");
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_Wait3SecondsAndExit)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::Wait3SecondsAndExit));
		const clock_t begin_time = clock();
		runner.RunAndWait(argument);
		float seconds = float(clock() - begin_time) / CLOCKS_PER_SEC;
		ASSERT_TRUE(seconds > 2);
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_PrintArguments)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		std::vector<std::string> arguments{
			std::to_string(static_cast<int>(CommandType::PrintArguments)),
			"1",
			"---",
			"qwerty"
		};
		auto lines = runner.RunAndWait(arguments);
		ASSERT_EQ(lines[0], arguments[0]);
		ASSERT_EQ(lines[1], arguments[1]);
		ASSERT_EQ(lines[2], arguments[2]);
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_Read3Lines)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::Read3Lines));
		runner.Run(argument);
		runner.WriteLine("1");
		runner.WriteLine("2");
		runner.WriteLine("3");
		runner.WaitForExit();
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_ReadWrite3Lines)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::ReadWrite3Lines));
		runner.Run(argument);
		runner.WriteLine("1");
		auto line = runner.ReadLine();
		ASSERT_EQ(line, "This is line #1");
		runner.WriteLine("2");
		line = runner.ReadLine();
		ASSERT_EQ(line, "This is line #2");
		runner.WriteLine("3");
		line = runner.ReadLine();
		ASSERT_EQ(line, "This is line #3");
		runner.WaitForExit();
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_ReadAndWriteReverse)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::ReadAndWriteReverse));
		runner.Run(argument);
		runner.WriteLine("3");
		runner.WriteLine("qwerty");
		auto line = runner.ReadLine();
		ASSERT_EQ(line, "ytrewq");
		runner.WriteLine("12345");
		line = runner.ReadLine();
		ASSERT_EQ(line, "54321");
		runner.WriteLine("-");
		line = runner.ReadLine();
		ASSERT_EQ(line, "-");
		runner.WaitForExit();
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_ReadToEnd)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::Print5Lines));
		runner.Run(argument);
		auto output = runner.ReadToEnd();
		auto expected = "This is line #1\r\n" \
			"This is line #2\r\n" \
			"This is line #3\r\n" \
			"This is line #4\r\n" \
			"This is line #5\r\n";
		ASSERT_EQ(expected, output);
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_ExitCode111)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::ExitCode111));
		runner.RunAndWait(argument);
		auto exit_code = runner.GetExitCode();
		ASSERT_EQ(111, exit_code);
	}

	TEST(ProcessRunnerTests, ProcessRunnerTest_ExitCode1234)
	{
		utils::ProcessRunner runner(TEST_PROGRAM_PATH);
		auto argument = std::to_string(static_cast<int>(CommandType::ExitCode1234));
		runner.RunAndWait(argument);
		auto exit_code = runner.GetExitCode();
		ASSERT_EQ(1234, exit_code);
	}
}