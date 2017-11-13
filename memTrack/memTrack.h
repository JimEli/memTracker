// Simple debug print malloc/free functions.
// Do not explicitly include this header, use memTracker.h instead.
// Version 2.2
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

// Memory allocation stats.
typedef struct BLOCKINFO {
	struct BLOCKINFO *pbiNext; // Pointer to next element in structure.
	uint8_t *pb;               // Memory pointer.
	size_t size;               // Size of requested block.
	bool free;                 // True if memory has been released.
} blockinfo;

// Memory allocation is expanded by padding amount (equally spaced before/after actual).
#define MALLOC_PADDING        16
#define MALLOC_PADDING_LENGTH (MALLOC_PADDING / 2)
#define MALLOC_START_OFFSET   (MALLOC_PADDING / 2)

// Memory paint values.
static unsigned char _deadLandFill = 0xDD;  // Fill free objects with this value.
static unsigned char _cleanLandFill = 0xCC; // Fill new objects with this value.

// Memory statistics.
static size_t totalMemory = 0;

// Redirection function definitions.
void *__Malloc(size_t, char *, int);
void __Free(void *, char *, int);
void __Exit(int const);

// Internal function definitions.
static void clearMemoryStatus(void);
static void setMemoryStatus(void *pv);
static blockinfo *getBlockInfo(uint8_t *pMem);
static bool createBlockInfo(uint8_t *pNewMem, size_t sizeNew);
static size_t sizeOfBlock(uint8_t *pMem);

#endif
#endif
