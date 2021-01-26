#include <string>
#include <sstream>
#include <Windows.h>

namespace utils
{
	class ProcessRunner
	{
	public:
		ProcessRunner(std::string filename);
		virtual ~ProcessRunner();
		std::string Run();
		std::string Run(const std::string& input);

	protected:


	private:
		const std::string _filename;

		std::string _output;
		HANDLE _child_std_in_read = NULL;
		HANDLE _child_std_in_write = NULL;
		HANDLE _child_std_out_read = NULL;
		HANDLE _child_std_out_write = NULL;

		void ProcessRunner::WriteToProcessInput(const std::string& input);
		void ProcessRunner::ReadProcessOutput();
		void Close(HANDLE& handle);
	};
}