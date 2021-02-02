#include <string>
#include <sstream>
#include <vector>
#include <Windows.h>

namespace utils
{
	class ProcessRunner
	{
	public:
		ProcessRunner(std::string filename);
		virtual ~ProcessRunner();

		void Run();
		void Run(const std::string& arguments);
		void Run(const std::vector<std::string>& arguments);
		std::vector<std::string> WaitForExit();
		std::vector<std::string> RunAndWait();
		std::vector<std::string> RunAndWait(const std::string& arguments);
		std::vector<std::string> RunAndWait(const std::vector<std::string>& arguments);
		void Terminate();
		DWORD GetProcessId() const;
		HANDLE GetProcessHandle() const;

		void WriteLine(const std::string& input);
		bool ReadLine(std::string& line);
		std::string ReadLine();

	protected:


	private:
#ifdef _WIN32
		const std::string END_LINE = "\r\n";
#endif
		static const int BUFFER_SIZE = 4096;
		const std::string m_filename;
		PROCESS_INFORMATION m_process_information;
		bool m_is_process_output_ended;
		std::string m_last_output;
		HANDLE m_child_std_in_read = NULL;
		HANDLE m_child_std_in_write = NULL;
		HANDLE m_child_std_out_read = NULL;
		HANDLE m_child_std_out_write = NULL;

		void Close(HANDLE& handle);
	};
}