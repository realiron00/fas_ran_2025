#include "ran.h"

int get_enc_size(const char* filename, int number)
{
	char encfilename[MAX_PATH]; // stores the encrypted filename
	snprintf(encfilename, sizeof(encfilename), "%d_%s.fas", number, filename);

	int size; // Size of the file
	FILE* fp = fopen(encfilename, "rb"); // Open the file
	if (fp == NULL) {
		printf("ERROR!\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	fclose(fp);
	return size;
}

void make_encfile(const char* filename, int number, unsigned char* key1, unsigned char* key2, unsigned char* iv, int filesize, int enc_filesize)
{
	char encfilename[MAX_PATH]; // stores the encrypted filename
	snprintf(encfilename, sizeof(encfilename), "%d_%s.fas", number, filename);

	FILE* fp = fopen(encfilename, "ab"); // Open file to be encrypted
	if (fp == NULL) {
		printf("ERROR!\n");
		return;
	}

	fwrite(key1, sizeof(char), 512, fp);

	fwrite(key2, sizeof(char), 512, fp);

	fwrite(iv, sizeof(char), 16, fp);

	fprintf(fp, "##");
	fprintf(fp, "%d", filesize);

	fprintf(fp, "##");
	fprintf(fp, "%d", enc_filesize);
	
	fclose(fp);
}