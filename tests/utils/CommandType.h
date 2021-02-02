namespace process_runner_tests
{
	enum class CommandType
	{
		None = 0,
		EmptyOutput,
		Wait3SecondsAndExit,
		Print5Lines,
		Read3Lines,
		ReadWrite3Lines,
		ReadAndWriteReverse,
		PrintArguments
	};
}