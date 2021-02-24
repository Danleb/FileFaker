#include <gtest/gtest.h>

namespace file_faker_tests
{
	class FileFakerTestsSameProcessF : public ::testing::Test
	{
	protected:
		const char* TrueInfo = "This is a file with true info.";
		const char* FakedInfo = "This is a file with faked info.";
		const char* TestProgram = "TestFileReaderProgram.exe";

		std::vector<CHAR> true_info_file_path;
		std::wstring wtrue_info_file_path;
		std::vector<CHAR> faked_info_file_path;
		REDIRECTION_HANDLE redirection_handle = INVALID_REDIRECTION_HANDLE;
		DWORD process_id;

		void SetUp() override;
		void FileFakerTestsSameProcessF::CreateTestFiles(std::vector<CHAR>& true_info_file_path, std::vector<CHAR>& faked_info_file_path);

	private:
		const char* TrueInfoFileName = "1.txt";
		const char* FakedInfoFileName = "2.txt";
	};
}