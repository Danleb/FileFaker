#include <fstream>
#include <gtest/gtest.h>
#include <Windows.h>

#include "file_faker_server.h"
#include "ProcessRunner.h"

namespace file_faker_tests
{
	const char* TestProgram = "TestFileReaderProgram.exe";
	const char* TrueInfoFileName = "1.txt";
	const char* FakedInfoFileName = "2.txt";
	const char* TrueInfo = "This is a file with true info.";
	const char* FakedInfo = "This is a file with faked info.";

	enum class FileFunctionType
	{
		fopen
	};

	void CreateFiles()
	{
		std::ofstream file(TrueInfoFileName);
		file << TrueInfo;
		file.close();

		file.open(FakedInfoFileName);
		file << FakedInfo;
		file.close();
	}

	TEST(FileFakerTests, EmptyTest)
	{
		EXPECT_EQ(true, true);
	}

	TEST(FileFakerTests, fopen_Test)
	{
		CreateFiles();
		utils::ProcessRunner runner(TestProgram);
		runner.Run();
		redirect_file_io(runner.GetProcessId(), TrueInfoFileName, FakedInfoFileName);
		Sleep(100);
		runner.WriteLine(TrueInfoFileName);
		std::string text_from_file = runner.ReadLine();
		ASSERT_EQ(FakedInfo, text_from_file);
	}
}