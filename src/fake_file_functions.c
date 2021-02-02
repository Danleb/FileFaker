#include <Windows.h>
#include <string.h>

#include "file_faker.h"

HFILE FakeOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	DWORD pid = GetCurrentProcessId();
	for (size_t i = 0; i < redirection_datas_count; ++i)
	{
		RedirectionData redirection_data = redirection_datas[i];
		if (redirection_data.pid == pid &&
			strcmp(redirection_data.file_path_from, lpFileName))
		{

		}
	}

	OpenFile(lpFileName, lpReOpenBuff, uStyle);
}