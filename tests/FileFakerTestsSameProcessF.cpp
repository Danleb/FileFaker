#include <fstream>
#include <gtest/gtest.h>
#include <Windows.h>
#include <codecvt>
#include "file_faker_server.h"
#include "FileFakerTestsSameProcessF.h"

namespace file_faker_tests
{
	void FileFakerTestsSameProcessF::SetUp()
	{
		CreateTestFiles(true_info_file_path, faked_info_file_path);
		process_id = GetCurrentProcessId();
		redirection_handle = redirect_file_io(process_id, true_info_file_path.data(), faked_info_file_path.data());
	}

	std::wstring ConvertAnsiToWide(const std::string& str)
	{
		int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);
		return wstr;
	}

	void FileFakerTestsSameProcessF::CreateTestFiles(std::vector<CHAR>& true_info_file_path, std::vector<CHAR>& faked_info_file_path)
	{
		std::ofstream file(TrueInfoFileName);
		file << TrueInfo;
		file.close();

		file.open(FakedInfoFileName);
		file << FakedInfo;
		file.close();

		true_info_file_path.resize(FILE_PATH_SIZE);
		GetFullPathName(TrueInfoFileName, FILE_PATH_SIZE, true_info_file_path.data(), NULL);
		wtrue_info_file_path = std::wstring(true_info_file_path.begin(), true_info_file_path.end());

		faked_info_file_path.resize(FILE_PATH_SIZE);
		GetFullPathName(FakedInfoFileName, FILE_PATH_SIZE, faked_info_file_path.data(), NULL);
	}

	TEST_F(FileFakerTestsSameProcessF, fopen_Test_SameProcess)
	{
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		FILE* file = fopen(true_info_file_path.data(), "r");
		char buffer[1000];
		fgets(buffer, 1000, file);
		fclose(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST_F(FileFakerTestsSameProcessF, _wfopen_Test_SameProcess)
	{
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		FILE* file = _wfopen(wtrue_info_file_path.c_str(), L"r");
		char buffer[1000];
		fgets(buffer, 1000, file);
		fclose(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST_F(FileFakerTestsSameProcessF, fopen_s_Test_SameProcess)
	{
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		FILE* file = nullptr;
		errno_t err = fopen_s(&file, true_info_file_path.data(), "r");
		ASSERT_EQ(err, EXIT_SUCCESS);
		ASSERT_NE(file, nullptr);
		char buffer[1000];
		fgets(buffer, 1000, file);
		fclose(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST_F(FileFakerTestsSameProcessF, _fsopen_Test_SameProcess)
	{
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		FILE* file = _fsopen(true_info_file_path.data(), "r", _SH_DENYNO);
		char buffer[1000];
		fgets(buffer, 1000, file);
		fclose(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST_F(FileFakerTestsSameProcessF, CreateFileA_Test_SameProcess)
	{
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		HANDLE file = CreateFileA(true_info_file_path.data(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		ASSERT_NE(file, INVALID_HANDLE_VALUE);
		const int BUFFER_SIZE = 1000;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		DWORD bytes_read;
		BOOL success = ReadFile(file, buffer, BUFFER_SIZE, &bytes_read, NULL);
		ASSERT_EQ(true, success);
		CloseHandle(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST_F(FileFakerTestsSameProcessF, CreateFileW_Test_SameProcess)
	{
		ASSERT_NE(redirection_handle, INVALID_REDIRECTION_HANDLE);

		HANDLE file = CreateFileW(wtrue_info_file_path.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		ASSERT_NE(file, INVALID_HANDLE_VALUE);
		const int BUFFER_SIZE = 1000;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		DWORD bytes_read;
		BOOL success = ReadFile(file, buffer, BUFFER_SIZE, &bytes_read, NULL);
		ASSERT_EQ(true, success);
		CloseHandle(file);

		std::string text_from_file(buffer);
		ASSERT_EQ(text_from_file, FakedInfo);
	}

	TEST_F(FileFakerTestsSameProcessF, std_ifstream_Test_SameProcess)
	{
		std::ifstream file(true_info_file_path.data());
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();
		std::string text_from_file(buffer.str());
		ASSERT_EQ(text_from_file, FakedInfo);
	}
}
