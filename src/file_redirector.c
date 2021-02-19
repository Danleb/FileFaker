#include <stdlib.h>

#include "file_redirector.h"

size_t local_redirections_count = 0;
RedirectionData* local_redirection_datas = NULL;

bool add_redirection(RedirectionData redirection_data)
{
	if (local_redirections_count == 0)
	{
		local_redirections_count = 1;
		local_redirection_datas = malloc(sizeof(RedirectionData));
		if (local_redirection_datas == NULL)
		{
			return false;
		}
		local_redirection_datas[0] = redirection_data;
		return true;
	}

	if (!redirection_data.file_from_defined)
	{
		for (size_t i = 0; i < local_redirections_count; ++i)
		{
			RedirectionData data = local_redirection_datas[i];
			if (!data.file_from_defined)
			{
				local_redirection_datas[i] = redirection_data;
				return true;
			}
		}

		return false;
	}

	RedirectionData* new_datas = malloc(sizeof(RedirectionData) * (local_redirections_count + 1));
	memcpy(new_datas, local_redirection_datas, sizeof(RedirectionData) * local_redirections_count);
	new_datas[local_redirections_count] = redirection_data;

	free(local_redirection_datas);
	local_redirection_datas = new_datas;

	return false;
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
			RedirectionData* new_datas = malloc(sizeof(RedirectionData) * (local_redirections_count - 1));
			if (new_datas == NULL)
			{
				return false;
			}
			memcpy(new_datas, local_redirection_datas, sizeof(RedirectionData) * i);
			memcpy(new_datas + i, local_redirection_datas + i + 1, sizeof(RedirectionData) * (local_redirections_count - i - 1));

			local_redirections_count--;
			free(local_redirection_datas);
			local_redirection_datas = new_datas;

			return true;
		}
	}

	return false;
}