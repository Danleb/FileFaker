#include <stdlib.h>

#include "redirections_manager.h"

size_t current_redirection_id = 1;
size_t local_redirections_count = 0;
RedirectionData* local_redirection_datas = NULL;

REDIRECTION_HANDLE getNextRedirectionHandle()
{
	return current_redirection_id++;
}

REDIRECTION_HANDLE add_redirection(RedirectionData redirection_data)
{
	if (local_redirections_count == 0)
	{
		local_redirections_count = 1;
		local_redirection_datas = malloc(sizeof(RedirectionData));
		if (local_redirection_datas == NULL)
		{
			return INVALID_REDIRECTION_HANDLE;
		}

		REDIRECTION_HANDLE new_redirection_handle = getNextRedirectionHandle();
		redirection_data.handle = new_redirection_handle;
		local_redirection_datas[0] = redirection_data;
		return new_redirection_handle;
	}

	if (!redirection_data.file_from_defined)
	{
		for (size_t i = 0; i < local_redirections_count; ++i)
		{
			RedirectionData* data = &local_redirection_datas[i];
			if (!data->file_from_defined)
			{
				REDIRECTION_HANDLE new_redirection_handle = getNextRedirectionHandle();
				redirection_data.handle = new_redirection_handle;
				local_redirection_datas[i] = redirection_data;
				return new_redirection_handle;
			}
		}

		return INVALID_REDIRECTION_HANDLE;
	}

	RedirectionData* new_datas = malloc(sizeof(RedirectionData) * (local_redirections_count + 1));
	if (new_datas == NULL)
	{
		return INVALID_REDIRECTION_HANDLE;
	}
	memcpy(new_datas, local_redirection_datas, sizeof(RedirectionData) * local_redirections_count);

	REDIRECTION_HANDLE new_redirection_handle = getNextRedirectionHandle();
	redirection_data.handle = new_redirection_handle;
	new_datas[local_redirections_count] = redirection_data;

	free(local_redirection_datas);
	local_redirection_datas = new_datas;

	return new_redirection_handle;
}

bool remove_redirection(REDIRECTION_HANDLE redirection_handle)
{
	if (local_redirection_datas == NULL)
	{
		return false;
	}

	for (size_t i = 0; i < local_redirections_count; ++i)
	{
		RedirectionData* data = &local_redirection_datas[i];
		if (data->handle == redirection_handle)
		{
			if (local_redirections_count == 1)
			{
				free(local_redirection_datas);
				local_redirection_datas = NULL;
				local_redirections_count = 0;
				return true;
			}
			else
			{
				RedirectionData* new_datas = malloc(sizeof(RedirectionData) * (local_redirections_count - 1));
				if (new_datas == NULL)
				{
					return false;
				}
				memcpy(new_datas, local_redirection_datas, sizeof(RedirectionData) * i);
				memcpy(new_datas + i, local_redirection_datas + i + 1, sizeof(RedirectionData) * (local_redirections_count - i - 1));
				free(local_redirection_datas);
				local_redirection_datas = new_datas;
				local_redirections_count--;
			}

			return true;
		}
	}

	return false;
}