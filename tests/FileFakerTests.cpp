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

	void CreateTestFiles(std::vector<CHAR>& true_info_file_path, std::vector<CHAR>& faked_info_file_path)
	{
		std::ofstream file(TrueInfoFileName);
		file << TrueInfo;
		file.close();

		file.open(FakedInfoFileName);
		file << FakedInfo;
		file.close();

		true_info_file_path.resize(FILE_PATH_SIZE);
		GetFullPathName(TrueInfoFileName, FILE_PATH_SIZE, true_info_file_path.data(), NULL);
		faked_info_file_path.resize(FILE_PATH_SIZE);
		GetFullPathName(FakedInfoFileName, FILE_PATH_SIZE, faked_info_file_path.data(), NULL);
	}

	TEST(FileFakerTests, EmptyTest)
	{
		EXPECT_EQ(true, true);
	}

	TEST(FileFakerTests, fopen_Test_SameProcess)
	{
		DWORD pid = GetCurrentProcessId();
		std::vector<CHAR> true_info_file_path;
		std::vector<CHAR> faked_info_file_path;
		CreateTestFiles(true_info_file_path, faked_info_file_path);
		REDIRECTION_HANDLE redirection_handle = redirect_file_io(pid, true_info_file_path.data(), faked_info_file_path.data());
	}

	TEST(FileFakerTests, fopen_Test)
	{
		std::vector<CHAR> true_info_file_path;
		std::vector<CHAR> faked_info_file_path;
		CreateTestFiles(true_info_file_path, faked_info_file_path);
		utils::ProcessRunner runner(TestProgram);
		runner.Run();
		REDIRECTION_HANDLE redirection_handle = redirect_file_io(runner.GetProcessId(), true_info_file_path.data(), faked_info_file_path.data());
		//Sleep(100);
		runner.WriteLine(TrueInfoFileName);
		std::string text_from_file = runner.ReadToEnd();
		runner.WaitForExit();
		ASSERT_NE(INVALID_REDIRECTION_HANDLE, redirection_handle);
		ASSERT_EQ(runner.GetExitCode(), EXIT_SUCCESS);
		ASSERT_EQ(FakedInfo, text_from_file);
	}
}