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
		std::vector<CHAR> true_info_file_path;
		std::vector<CHAR> faked_info_file_path;
		CreateTestFiles(true_info_file_path, faked_info_file_path);

		DWORD pid = GetCurrentProcessId();
		REDIRECTION_HANDLE redirection_handle = redirect_file_io(pid, true_info_file_path.data(), faked_info_file_path.data());
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		FILE* file = fopen(true_info_file_path.data(), "r");
		char buffer[1000];
		fgets(buffer, 1000, file);
		fclose(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST(FileFakerTests, fopen_Test)
	{
		//std::vector<CHAR> true_info_file_path;
		//std::vector<CHAR> faked_info_file_path;
		//CreateTestFiles(true_info_file_path, faked_info_file_path);
		//utils::ProcessRunner runner(TestProgram);
		//runner.Run();
		//REDIRECTION_HANDLE redirection_handle = redirect_file_io(runner.GetProcessId(), true_info_file_path.data(), faked_info_file_path.data());
		////Sleep(100);
		//runner.WriteLine(TrueInfoFileName);
		//std::string text_from_file = runner.ReadToEnd();
		//runner.WaitForExit();
		//ASSERT_NE(INVALID_REDIRECTION_HANDLE, redirection_handle);
		//ASSERT_EQ(runner.GetExitCode(), EXIT_SUCCESS);
		//ASSERT_EQ(FakedInfo, text_from_file);
	}

	//std::ifstream file(TrueInfoFileName);
	//std::stringstream buffer;
	//buffer << file.rdbuf();
}