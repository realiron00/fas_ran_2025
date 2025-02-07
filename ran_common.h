#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#pragma comment(lib, "bcrypt.lib")

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)

#define SUCCESS 1
#define FAIL 0

#define _CRT_SECURE_NO_WARNINGS 

#pragma warning(disable:4996)