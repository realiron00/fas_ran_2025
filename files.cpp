#include "ran.h"

const char* extensions[] = { "txt", "pdf", "hwp", "pptx", "ppt", "docx", "xlsx", "jpg", "png" }; // List of extensions to search 
int numExtensions = sizeof(extensions) / sizeof(extensions[0]); // Number of extensions to search

void file_search(char filenames[][MAX_PATH], int* filecount)
{
	char directory[4096]; // Current directory
	_getcwd(directory, sizeof(directory));

	WIN32_FIND_DATAA finddata;
	HANDLE hfind;

	char path[MAX_PATH];

	// Search for files with the given extensions
	for (int i = 0; i < numExtensions; ++i) {
		sprintf(path, "%s\\*.%s", directory, extensions[i]); // Create the search path

		hfind = FindFirstFileA(path, &finddata);
		if (hfind == INVALID_HANDLE_VALUE) {
			printf("No files found.\n");
			continue;
		}

		else if (hfind != INVALID_HANDLE_VALUE) {
			// Store the filenames
			do {
				strcpy(filenames[*filecount], finddata.cFileName);
				(*filecount)++;
			} while (FindNextFileA(hfind, &finddata) && (*filecount < 100)); // Limit the number of files to 100

			FindClose(hfind);
		}
	}
}

int get_file_size(const char filenames[][MAX_PATH], int filenumber)
{
	int size; // Size of the file
	FILE* fp = fopen(filenames[filenumber], "rb"); // Open the file
	if (fp == NULL) {
		printf("ERROR!\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	fclose(fp);
	return size;
}