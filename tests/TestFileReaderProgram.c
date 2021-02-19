#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 2048

int main()
{
	char file_path[BUFFER_SIZE];
	int result = scanf("%s", file_path);
	file_path[BUFFER_SIZE - 1] = 0;

	if (result != 1)
	{
		printf("Failed to read file path.");
		return EXIT_FAILURE;
	}

	FILE* file_handle = fopen(file_path, "r");
	if (file_handle == NULL)
	{
		printf("Failed to open file by path: %s", file_path);
		return EXIT_FAILURE;
	}

	char c = fgetc(file_handle);
	while (c != EOF)
	{
		printf("%c", c);
		c = fgetc(file_handle);
	}
	fclose(file_handle);
	return EXIT_SUCCESS;
}