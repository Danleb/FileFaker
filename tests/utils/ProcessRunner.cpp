#include <stdexcept>
#include <stdio.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>

#include "ProcessRunner.h"

namespace utils
{
	ProcessRunner::ProcessRunner(std::string filename) : _filename(filename)
	{

	}

	ProcessRunner::~ProcessRunner()
	{
		Close(_child_std_in_write);
		Close(_child_std_out_read);
	}

	std::string ProcessRunner::Run()
	{
		return Run("");
	}

	std::string ProcessRunner::Run(const std::string& input)
	{
		SECURITY_ATTRIBUTES security_attributes;
		security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		security_attributes.lpSecurityDescriptor = NULL;
		security_attributes.bInheritHandle = TRUE;

		if (!CreatePipe(&_child_std_out_read, &_child_std_out_write, &security_attributes, 0))
		{
			throw std::runtime_error("Failed to create pipe for std::out.");
		}
		if (!SetHandleInformation(_child_std_out_read, HANDLE_FLAG_INHERIT, 0))
		{
			throw std::runtime_error("Failed to SetHandleInformation.");
		}
		if (!CreatePipe(&_child_std_in_read, &_child_std_in_write, &security_attributes, 0))
		{
			throw std::runtime_error("Failed to create pipe for std::in.");
		}
		if (!SetHandleInformation(_child_std_in_write, HANDLE_FLAG_INHERIT, 0))
		{
			throw std::runtime_error("Failed to SetHandleInformation.");
		}

		PROCESS_INFORMATION process_information;
		ZeroMemory(&process_information, sizeof(PROCESS_INFORMATION));

		STARTUPINFO startup_info;
		ZeroMemory(&startup_info, sizeof(STARTUPINFO));
		startup_info.cb = sizeof(STARTUPINFO);
		startup_info.hStdError = _child_std_out_write;
		startup_info.hStdOutput = _child_std_out_write;
		startup_info.hStdInput = _child_std_in_read;
		startup_info.dwFlags |= STARTF_USESTDHANDLES;

		const size_t s = 1000;
		CHAR buffer[s];
		GetCurrentDirectory(s, buffer);

		BOOL success = CreateProcess(_filename.c_str(),
			NULL,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&startup_info,
			&process_information);

		if (!success)
		{
			throw std::runtime_error("Failed to create process.");
		}
		else
		{
			// Close handles to the child process and its primary thread.
			// Some applications might keep these handles to monitor the status
			// of the child process, for example. 
			
			CloseHandle(process_information.hProcess);
			CloseHandle(process_information.hThread);

			//// Close handles to the stdin and stdout pipes no longer needed by the child process.
			//// If they are not explicitly closed, there is no way to recognize that the child process has ended.
			CloseHandle(_child_std_out_write);
			CloseHandle(_child_std_in_read);
		}
		
		//
		//WriteToProcessInput(input);
		//WaitForSingleObject(process_information.hProcess, INFINITE);
		ReadProcessOutput();
		return _output;
	}

	void ProcessRunner::WriteToProcessInput(const std::string& input)
	{
		BOOL success = WriteFile(_child_std_in_write, input.c_str(), input.size(), 0, NULL);
		if (!success)
		{
			throw std::runtime_error("Failed to pass input string to child process pipe.");
		}
		Close(_child_std_in_write);
	}

	void ProcessRunner::ReadProcessOutput()
	{
		const size_t LIBRARY_PATH_BUFFER_SIZE = 4096;
		CHAR buffer[LIBRARY_PATH_BUFFER_SIZE];
		DWORD bytes_read;

		while (true)
		{
			BOOL success = ReadFile(_child_std_out_read, buffer, LIBRARY_PATH_BUFFER_SIZE, &bytes_read, NULL);
			if (!success)
			{
				DWORD error = GetLastError();
				if (error == ERROR_BROKEN_PIPE)
				{
					break;
				}
				throw std::runtime_error("Failed to read from child process std::out.");
			}
			if (bytes_read == 0)
			{
				break;
			}
			_output.append(buffer, bytes_read);
		}
	}

	void ProcessRunner::Close(HANDLE& handle)
	{
		if (handle != NULL)
		{
			BOOL success = CloseHandle(handle);
			if (!success)
			{
				throw std::runtime_error("Failed to close handle.");
			}
		}
		handle = NULL;
	}
}