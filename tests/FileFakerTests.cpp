#include <gtest/gtest.h>
#include <Windows.h>

//#include "ProcessRunner.h"

namespace file_faker_tests
{
	TEST(FileFakerTests, FileReaderTest)
	{
		/*utils::ProcessRunner runner("TestFileReaderProgram");
		std::string output = runner.Run("1.txt");
	//	ASSERT_EQ(output, "Empty input");*/
		EXPECT_EQ(true, false);
    }

	TEST(FileFakerTests, ZeroTest)
	{
		ASSERT_DOUBLE_EQ(1.0, 1.000001);
	}

	TEST(ABC, TEST1) {
		EXPECT_EQ(true, true);
	}

	TEST(ABC, TEST2) {
		EXPECT_EQ(true, true);
	}
}