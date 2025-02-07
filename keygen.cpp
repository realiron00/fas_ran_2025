#include "ran.h"

void gen_key(unsigned char* key_out, int key_len)
{
	if (key_out == NULL || key_len <= 0) {
		printf("Invalid output buffer\n");
		return;
	}

	NTSTATUS status;
	BCRYPT_ALG_HANDLE algHandle = NULL;

	status = BCryptOpenAlgorithmProvider(
		&algHandle,								//Address of Algorithm Handle
		BCRYPT_RNG_ALGORITHM,					//Cryptographic Algorithm Name
		NULL,									//Not use
		0);										//Flags
	if (!NT_SUCCESS(status)) {
		printf("Error code: %08x\n", status);
		return;
	}

	status = BCryptGenRandom(
		algHandle,								//Algorithm Handle
		key_out,									//Address of Buffer
		key_len,								//Buffer Size
		0);		//Flags
	if (!NT_SUCCESS(status)) {
		printf("Error code: %08x\n", status);
		return;
	}

	status = BCryptCloseAlgorithmProvider(algHandle, 0);
}