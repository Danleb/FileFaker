#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <strsafe.h>
#include <sstream>
#include <tchar.h>
#include <Windows.h>

#include "ProcessRunner.h"

namespace utils
{
	ProcessRunner::ProcessRunner(std::string filename) : m_filename(filename), m_is_process_output_ended(false)
	{
		ZeroMemory(&m_process_information, sizeof(PROCESS_INFORMATION));
	}

	ProcessRunner::~ProcessRunner()
	{
		if (m_process_information.hProcess != NULL)
		{
			Terminate();
		}
		Close(m_child_std_in_read);
		Close(m_child_std_in_write);
		Close(m_child_std_out_read);
		Close(m_child_std_out_write);
	}

	void ProcessRunner::Run()
	{
		Run("");
	}

	void ProcessRunner::Run(const std::vector<std::string>& arguments)
	{
		std::stringstream string_builder;
		for (size_t i = 0; i < arguments.size() - 1; ++i)
		{
			string_builder << arguments[i] << " ";
		}
		string_builder << arguments.back();
		std::string argument = string_builder.str();
		Run(argument);
	}

	void ProcessRunner::Run(const std::string& arguments)
	{
		SECURITY_ATTRIBUTES security_attributes;
		security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		security_attributes.lpSecurityDescriptor = NULL;
		security_attributes.bInheritHandle = TRUE;

		if (!CreatePipe(&m_child_std_out_read, &m_child_std_out_write, &security_attributes, 0))
		{
			throw std::runtime_error("Failed to create pipe for std::out.");
		}
		if (!SetHandleInformation(m_child_std_out_read, HANDLE_FLAG_INHERIT, 0))
		{
			throw std::runtime_error("Failed to SetHandleInformation.");
		}
		if (!CreatePipe(&m_child_std_in_read, &m_child_std_in_write, &security_attributes, 0))
		{
			throw std::runtime_error("Failed to create pipe for std::in.");
		}
		if (!SetHandleInformation(m_child_std_in_write, HANDLE_FLAG_INHERIT, 0))
		{
			throw std::runtime_error("Failed to SetHandleInformation.");
		}

		STARTUPINFO startup_info;
		ZeroMemory(&startup_info, sizeof(STARTUPINFO));
		startup_info.cb = sizeof(STARTUPINFO);
		startup_info.hStdError = m_child_std_out_write;
		startup_info.hStdOutput = m_child_std_out_write;
		startup_info.hStdInput = m_child_std_in_read;
		startup_info.dwFlags |= STARTF_USESTDHANDLES;

		std::vector<CHAR> command_line(m_filename.begin(), m_filename.end());
		command_line.push_back(' ');
		command_line.insert(command_line.end(), arguments.begin(), arguments.end());
		command_line.push_back('\0');

		BOOL success = CreateProcess(m_filename.c_str(),
			command_line.data(),
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&startup_info,
			&m_process_information);

		if (!success)
		{
			throw std::runtime_error("Failed to create process.");
		}
		else
		{
			// Close handles to the stdin and stdout pipes no longer needed by the child process.
			// If they are not explicitly closed, there is no way to recognize that the child process has ended.
			Close(m_child_std_in_read);
			Close(m_child_std_out_write);
		}
	}

	std::vector<std::string> ProcessRunner::WaitForExit()
	{
		std::vector<std::string> output;
		while (true)
		{
			std::string line;
			if (ReadLine(line))
			{
				output.push_back(line);
			}
			else
			{
				break;
			}
		}

		WaitForSingleObject(m_process_information.hProcess, INFINITE);
		return output;
	}

	void ProcessRunner::WriteLine(const std::string& input)
	{
		std::vector<CHAR> buffer(input.begin(), input.end());
		buffer.insert(buffer.end(), END_LINE.begin(), END_LINE.end());		

		BOOL success = WriteFile(m_child_std_in_write, buffer.data(), buffer.size(), NULL, NULL);
		if (!success)
		{
			throw std::runtime_error("Failed to pass input string to child process pipe.");
		}
	}

	bool ProcessRunner::ReadLine(std::string& line)
	{
		std::string output = m_last_output;

		CHAR buffer[BUFFER_SIZE];
		DWORD bytes_read;

		while (true)
		{
			BOOL success = ReadFile(m_child_std_out_read, buffer, BUFFER_SIZE, &bytes_read, NULL);
			output.append(buffer, bytes_read);
			if (!success)
			{
				DWORD error = GetLastError();
				if (error == ERROR_BROKEN_PIPE)
				{
					m_is_process_output_ended = true;
				}
				else
				{
					throw std::runtime_error("Failed to read from child process std::out.");
				}
			}

			size_t new_line_index = output.find(END_LINE);
			if (new_line_index == std::string::npos)
			{
				if (m_is_process_output_ended)
				{
					return false;
				}
			}
			else
			{
				line = output.substr(0, new_line_index);
				m_last_output = output.substr(new_line_index + END_LINE.size(), std::string::npos);
				break;
			}
		}

		return true;
	}

	std::string ProcessRunner::ReadLine()
	{
		std::string line;
		if (ReadLine(line))
		{
			return line;
		}
		else
		{
			return "";
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

	HANDLE ProcessRunner::GetProcessHandle() const
	{
		return m_process_information.hProcess;
	}

	DWORD ProcessRunner::GetProcessId() const
	{
		return m_process_information.dwProcessId;
	}

	void ProcessRunner::Terminate()
	{
		BOOL success = TerminateProcess(m_process_information.hProcess, 0);
		if (!success)
		{
			DWORD exit_code;
			success = GetExitCodeProcess(m_process_information.hProcess, &exit_code);
			if (success)
			{
				if (exit_code == STILL_ACTIVE)
				{
					throw std::runtime_error("Failed to terminate child process.");
				}
			}
			else
			{
				throw std::runtime_error("Failed to get process exit code.");
			}
		}
		WaitForSingleObject(m_process_information.hProcess, INFINITE);
		Close(m_process_information.hProcess);
		Close(m_process_information.hThread);
	}

	std::vector<std::string> ProcessRunner::RunAndWait()
	{
		Run();
		return WaitForExit();
	}

	std::vector<std::string> ProcessRunner::RunAndWait(const std::string& arguments)
	{
		Run(arguments);
		return WaitForExit();
	}

	std::vector<std::string> ProcessRunner::RunAndWait(const std::vector<std::string>& arguments)
	{
		Run(arguments);
		return WaitForExit();
	}
}