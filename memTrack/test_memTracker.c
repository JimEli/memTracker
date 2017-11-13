// Simple memory tracker test code.
#include <stdlib.h>
#include <stdio.h>

// Add debug memory allocation routines.
#include "memTracker.h"

#define COUNT 3 //Number of character mallocs.

#ifdef _MSC_VER
// C/C++ Preprocessor Definitions: _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996) 
#endif

// Test memory structure.
struct test {
	char c;
	int i;
	double d;
};

int main(void) {
	// Pointers used for test memory allocation.
	struct test *pStruct;
	char *pChar[COUNT];
	int *pInt;

	// Required to make eclipse console output work properly.
	setvbuf(stdout, NULL, _IONBF, 0);
	fflush(stdout);

	fputs("Tesing debugMalloc & debugFree.\n\n", stdout);

	// Allocate some memory.
	for (int i = 0; i < COUNT; i++) {
		pChar[i] = (char *)malloc(sizeof(char));
		if (pChar[i] == NULL) {
			printf("Error allocating memory.\n");
			return EXIT_FAILURE;
		}
	}
	pInt = (int *)malloc(sizeof(int) * 1024);
	pStruct = (struct test *)malloc(sizeof(struct test));

	// fill allocated memory with value.
	*pChar[1] = 'A';
	// This line will cause an over-run error.
	*(pChar[0] + 1) = 'X';

	// Free some of the memory.
	fputs("\n", stdout);
	free(pInt);
	free(pStruct);
	// This loop doesn't free all the allocated memory, producing an error report on exit.
	for (int i = 0; i < COUNT - 1; i++)
//	for (int i = 0; i < COUNT; i++)
			if (pChar[i])
			free(pChar[i]);

	exit(EXIT_SUCCESS);
}
