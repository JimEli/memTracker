/*************************************************************************
* Title: memTracker test.
* File: test_memTracker.c
* Author: James Eli
* Date: 11/13/2017
*
* This program performs a simple test of the memTracker memory allocation
* tracking code. 
*
* 1. We allocate the following memory: 4 separate characters, 1 integer 
*    and 1 structure via calls to malloc().
* 2. To demonstrate under/over run detection, the program will access
*    memory above and below 2 of the character allocation addresses.
* 3. To demonstrate using a pointer after it has been released, we free 
*    the integer pointer and then store a value using it's invalid pointer.
* 4. Free is called twice using the same character pointer to activate the 
*    warning associated with multiple calls to free using the same pointer.
* 5. Finally, the program fails to free 1 character pointer prior to calling 
*    exit() in order to demonstrate the warning associated with a memory 
*    leak.
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141), using C
*      language options.
*  (2) Only compiles in the Debug version of project.
*  (3) Released into the public domain.
*************************************************************************
* Change Log:
*   11/13/2017: Initial release. JME
*************************************************************************/
#include <stdlib.h>
#include <stdio.h>

// Add debug memory allocation routines.
#include "memTracker.h"

#define COUNT 4 // Number individual character allocations.

#ifdef _MSC_VER
// C/C++ Preprocessor Definitions: _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996) 
#endif

// A simple test memory structure.
struct test {
	char c;
	int i;
	double d;
};

int main(void) {
	// Pointers used for testing memory allocation.
	struct test *pStruct;
	char *pChar[COUNT];
	int *pInt;

	// Allocate memory via calling malloc().
	for (int i = 0; i < COUNT; i++) {
		pChar[i] = (char *)malloc(sizeof(char));
		if (pChar[i] == NULL) {
			printf("Error allocating memory.\n");
			return EXIT_FAILURE;
		}
	}
	pInt = (int *)malloc(sizeof(int) * 1024);
	pStruct = (struct test *)malloc(sizeof(struct test));

	*pChar[0] = 'A';        // Fill allocated memory with value.
	*(pChar[1] - 1) = 'X';	// Causes an under-run error.
	*(pChar[2] + 1) = 'X';	// Causes an over-run error.

	free(pInt);
	*pInt = 0x12345678;	    // Causes warning for memory access after free().
	free(pStruct);
	free(pChar[0]);         // Causes a previous memory free warning. 

	// This loop doesn't free all the allocated memory, therefore producing a warning upon exit.
	for (int i = 0; i < COUNT - 1; i++)
		if (pChar[i])
			free(pChar[i]);
/*
	unsigned int *p = malloc(sizeof(unsigned int));
	*p = 0x12345678;
	unsigned int *np = realloc(p, 2*sizeof(unsigned int));
	free(np);
*/

	exit(EXIT_SUCCESS);
}
