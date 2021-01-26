#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		printf("Empty input");
		return EXIT_FAILURE;
	}
	else
	{
		printf(argv[1]);
	}
	
	char* file_path = argv[1];

	

	FILE* file_handle = fopen(file_path, "w");
	fprintf(file_handle, "Hello, World!");
	fclose(file_handle);

	return EXIT_SUCCESS;
}