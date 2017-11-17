/*************************************************************************
* Title: memTracker
* File: memTrack.h
* Author: James Eli
* Date: 11/13/2017your 
*
* This file includes the main functionality of the memTracker memory
* allocation tracking code.
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141), using C
*      language options.
*  (2) Not compiled in release version.
*  (3) Include the files memTrack.h, memTrack.c and memTracker.h in your 
*      project. Only include "memTracker.h" in your files. Do not explicitly 
*      include this header in your project files.
*  (4) Define VERBOSE to recieve malloc/free debug prints.
*  (5) Released into the public domain.
*************************************************************************
* Change Log:
*   11/13/2017: Initial release. JME
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#ifndef _DEBUG_MALLOC_H_
#define _DEBUG_MALLOC_H_

#ifdef _DEBUG

// Define VERBOSE_OUTPUT (below) to recieve malloc/free debug prints.
#define VERBOSE

// Memory allocation stats.
typedef struct BLOCKINFO {
	struct BLOCKINFO *pbiNext; // Pointer to next element in structure.
	uint8_t *pMem;             // Memory pointer.
	size_t size;               // Size of requested block.
	bool free;                 // True if memory has been released.
} blockinfo;

// Memory allocation is expanded by padding amount (equally spaced before/after actual).
#define MALLOC_PADDING        16 // Should be an even value.
#define MALLOC_PADDING_LENGTH (MALLOC_PADDING / 2)
#define MALLOC_START_OFFSET   (MALLOC_PADDING / 2)

// Memory paint values.
static unsigned char _cleanLandFill = 0xCC; // Fill new memory with this value.
static unsigned char _deadLandFill = 0xDD;  // Fill free memory with this value.

// Records total memory allocations.
static size_t totalMemory = 0;

// Redirection function definitions.
void *__Malloc(size_t, char *, int);
void *__Calloc(size_t, size_t, char *, int);
void *__Realloc(void *, size_t, char *, int);
void __Free(void *, char *, int);
void __Exit(int const);

// Additional function definitions (can be called outside of memTracker).
size_t sizeOfBlock(uint8_t *);

// Internal function definitions.
static bool setMemoryStatus(void *);
static bool createBlockInfo(uint8_t *, size_t);
static blockinfo *getBlockInfo(uint8_t *);
static void freeBlockInfo(uint8_t *);
static void checkAllocations(void);
static void *resizeMemory(void **, size_t, char *, int);

#endif

#endif
