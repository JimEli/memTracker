/*************************************************************************
* Title: memTracker.
* File: memTrack.c
* Author: James Eli
* Date: 11/13/2017
*
* This file includes the main functionality of the memTracker memory 
* allocation tracking code. The __Malloc, __Calloc, __Realloc, __Free and 
* __Exit functions intercept the user's calls to the respective system 
* functions (via replacement by macros inside memTracker.h). The remaining 
* functions below are used to track the user's memory allocations using the 
* very simple BLOCKINFO linked-list structure.
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141), using C
*      language options.
*  (2) Not compiled in release version.
*  (3) Released into the public domain.
*************************************************************************
* Change Log:
*   11/13/2017: Initial release. JME
*************************************************************************/
#include "memTrack.h"

// This is only compiled in debug version.
#ifdef _DEBUG

// Pointer to head of memory blockinfo list.
static blockinfo *pbiHead = NULL;

// Check and designate pointer as free.
bool setMemoryStatus(const void *pMem) {
	blockinfo *pbi;

	pbi = getBlockInfo((uint8_t *)pMem);
	if (pbi->status & BLOCK_STATUS_FREE)
		return false;
	pbi->status |= BLOCK_STATUS_FREE;
	return true;
}

// Check and designate pointer as free.
static unsigned char getBlockStatus(const void *pMem) {
	blockinfo *pbi;

	pbi = getBlockInfo((uint8_t *)pMem);
	if (pbi != NULL)
		return pbi->status;
	return 0;
}

// Create a new blockinfo list entry for memory pointer.
static bool createBlockInfo(uint8_t *pMem, const size_t size, const unsigned char status) {
	assert(pMem != NULL && size != 0);

	// Reserve memory for new blockinfo list entry.
	blockinfo *pbi = (blockinfo *)malloc(sizeof(blockinfo));

	// Fill structure.
	if (pbi != NULL) {
		pbi->pMem = pMem;
		pbi->size = size;
		pbi->status = status;
		pbi->pbiNext = pbiHead;
		pbiHead = pbi;
	}

	// Return status.
	return (bool)(pbi != NULL);
}

// Return blockinfo list element corresponding to memory pointer.
static blockinfo *getBlockInfo(const uint8_t *pMem) {
	blockinfo *pbi;

	assert(pMem != NULL);

	// Walk blockinfo list looking for match.
	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		uint8_t *pbStart = pbi->pMem;
		uint8_t *pbEnd = pbi->pMem + pbi->size - 1;
		if (pMem >= pbStart && pMem <= pbEnd)
			break;
	}

	assert(pbi != NULL);
	return (pbi);
}

// Update blockinfo entry for the memory pointer.
static void updateBlockInfo(const uint8_t *pOld, uint8_t *pNew, const size_t sizeNew, const unsigned char status) {
	assert(pNew != NULL && sizeNew != 0);

	blockinfo *pbi = getBlockInfo(pOld);
	assert(pOld == pbi->pMem);
	pbi->pMem = pNew;
	pbi->size = sizeNew;
	pbi->status |= status;
}

// Release the blockinfo entry for the memory pointer.
static void freeBlockInfo(const uint8_t *pMem) {
	blockinfo *pbi, *pbiPrev = NULL;

	// Walk the blockinfo list looking for match.
	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		if (pbi->pMem == pMem) {
			// Matched a block, update links.
			if (pbiPrev == NULL)
				pbiHead = pbi->pbiNext;
			else
				pbiPrev->pbiNext = pbi->pbiNext;
			break;
		}

		pbiPrev = pbi;
	}

 	assert(pbi != NULL);
	
	// Annotate blockinfo memory as dead.
	memset(pbi, _deadLandFill, sizeof(blockinfo));
	
	// Free the memory for this blockinfo list entry.
	free(pbi);
}

// Return size of memory block associated with pointer.
size_t sizeOfBlock(const uint8_t *pMem) {
	blockinfo *pbi = getBlockInfo(pMem);
	assert(pMem == pbi->pMem);
	return (pbi->size);
}

// Print report of _all_ memory allocations.
void reportAllocations(void) {
	blockinfo *pbi = pbiHead;

	while (pbi != NULL) {
		blockinfo *next = pbi->pbiNext;
		if (pbi->pMem != NULL) {
			// Block status descriptions.
			char *blockStatus[4] = { "malloc ", "calloc ", "realloc ", "free " };

			fprintf(stderr, "0x%X size: %d", (unsigned int)pbi->pMem, pbi->size);
			fputs(" [ ", stderr);
			// Check all status bits.
			for (uint8_t i = 0; i < MAX_STATUS_BITS; i++)
				if (pbi->status & (1 << i))
					fputs(blockStatus[i], stderr);
			fputs("]\n", stderr);

			pbi = next;
		}
	}
}

// Check _all_ released memory for invalid access.
static void checkAllocations(void) {
	blockinfo *pbi = pbiHead;

	// Walk the allocated memory blockinfo list.
	while (pbi != NULL) {
		// First get pointer to next block.
		blockinfo *next = pbi->pbiNext;

		// If block exists...
		if (pbi->pMem != NULL) {
			// Get size of memory block.
			size_t size = pbi->size;

			// Has memory been freed?
			if (!CHECK_BLOCK_FREE(pbi->status))
				fprintf(stderr, "*** WARNING: Memory not free'd at 0x%p.\n", pbi->pMem);
			else 
				// Check for dead memory access.
				for (uint16_t i = 0; i < size; i++)
					if (*(pbi->pMem + i) != _deadLandFill)
						fprintf(stderr, "*** WARNING: Free'd memory access detected at 0x%p.\n", pbi->pMem + i);

			// Free memory and blockinfo for this pointer.
			free(pbi->pMem - MALLOC_START_OFFSET);
			freeBlockInfo(pbi->pMem);

			// Process next block.
			pbi = next;
		}
	}
}

static void *resizeMemory(void **ppv, size_t sizeNew, char *file, int line) {
	uint8_t **ppb = (uint8_t **)ppv;
	uint8_t *pNew;
	size_t sizeOld = sizeOfBlock(*ppb);

	if (sizeNew < sizeOld)
		memset((*ppb) + sizeNew, _deadLandFill, sizeOld - sizeNew);
/*
	// This code will force realloc() to move new location.
	else if (sizeNew > sizeOld) {
		uint8_t *pForceNew;

		if ((pForceNew = __Malloc(sizeNew, file, line)) != NULL) {
			memcpy(pForceNew, *ppb, sizeOld);
			__Free(*ppb, file, line);
			*ppb = pForceNew;
		}
	}
*/
	pNew = (uint8_t *)realloc(*ppb - MALLOC_START_OFFSET, sizeNew + MALLOC_PADDING);
	if (pNew != NULL) {
		if (pNew != *ppb) {
			createBlockInfo((uint8_t *)pNew + MALLOC_START_OFFSET, sizeNew, getBlockStatus(*ppb) + BLOCK_STATUS_REALLOC);
			freeBlockInfo(*ppb);
		}
		else
			updateBlockInfo(*ppv, pNew + MALLOC_START_OFFSET, sizeNew, BLOCK_STATUS_REALLOC);

		if (sizeNew > sizeOld)
			memset(pNew + sizeOld, _cleanLandFill, sizeNew - sizeOld);
	}

	// Paint the memory padding.
	memset(pNew, _cleanLandFill, MALLOC_PADDING_LENGTH);
	memset((uint8_t *)pNew + MALLOC_PADDING_LENGTH + sizeNew, _cleanLandFill, MALLOC_PADDING_LENGTH);

	// Recalculate the total memory count.
	totalMemory = totalMemory - sizeOld + sizeNew;

#ifdef VERBOSE
	// Print statistics.
	fprintf(stderr, "realloc: %s, line #%d\n", file, line);
	fprintf(stderr, " 0x%p, size: %d, total: %d\n", (uint8_t *)pNew + MALLOC_START_OFFSET, sizeNew, totalMemory);
#endif

	// Return new pointer.
	return pNew + MALLOC_START_OFFSET;
}

// Our replacement for malloc().
void *__Malloc(size_t size, char *file, int line) {
	// Attempt to allocate requested size + our below/above padding.
	void *pMem = malloc(size + MALLOC_PADDING);

	// Attempt to create an info block for this memory.
	if (pMem != NULL && createBlockInfo((uint8_t *)pMem + MALLOC_START_OFFSET, size, BLOCK_STATUS_MALLOC)) {
		// Paint the memory as uninitailized.
		memset(pMem, _cleanLandFill, size + MALLOC_PADDING);

		// Keep count of total allocations.
		totalMemory += size;

#ifdef VERBOSE
		// Print statistics.
		fprintf(stderr, "malloc: %s, line #%d\n", file, line);
		fprintf(stderr, " 0x%p, size: %d, total: %d\n", (uint8_t *)pMem + MALLOC_START_OFFSET, size, totalMemory);
#endif

		// Return memory requested.
		return((uint8_t *)pMem + MALLOC_START_OFFSET);
	}

	// Failure.
	fprintf(stderr, "*** WARNING: malloc() failure: %s, line #%d\n", file, line);
	return NULL;
}

// Our replacement for calloc().
void *__Calloc(size_t num, size_t size, char *file, int line) {
	// Attempt to allocate requested size + our below/above padding.
	void *pMem = calloc(num + MALLOC_PADDING, size);

	// Attempt to create an info block for this memory.
	if (pMem != NULL && createBlockInfo((uint8_t *)pMem + MALLOC_START_OFFSET, num*size, BLOCK_STATUS_CALLOC)) {
		// Paint the memory padding.
		memset(pMem, _cleanLandFill, MALLOC_PADDING_LENGTH);
		memset((uint8_t *)pMem + MALLOC_PADDING_LENGTH + num*size, _cleanLandFill, MALLOC_PADDING_LENGTH);

		// Keep count of total allocations.
		totalMemory += num*size;

#ifdef VERBOSE
		// Print statistics.
		fprintf(stderr, "calloc: %s, line #%d\n", file, line);
		fprintf(stderr, " 0x%p, size: %d, total: %d\n", (uint8_t *)pMem + MALLOC_START_OFFSET, num*size, totalMemory);
#endif

		// Return memory requested.
		return((uint8_t *)pMem + MALLOC_START_OFFSET);
	}

	// Failure.
	fprintf(stderr, "*** WARNING: calloc() failure: %s, line #%d\n", file, line);
	return NULL;
}

// Our replacement for realloc().
void *__Realloc(void *pMem, size_t size, char *file, int line) {
	// If size is zero, free the memory.
	if (size == 0) {
#ifdef VERBOSE
		// Present warning that realloc called with size zero, which is same as calling free().
		fprintf(stderr, "*** WARNING: realloc() called with 0 size: %s, line #%d\n", file, line);
#endif
		__Free(pMem, file, line);
		return NULL;
	}

	// If pointer is NULL, then allocate new memory.
	if (pMem == NULL) {
#ifdef VERBOSE
		// Present warning realloc call with NULL pointer, which is same as calling malloc().
		fprintf(stderr, "*** WARNING: realloc() called with NULL pointer: %s, line #%d\n", file, line);
#endif
		pMem = __Malloc(size, file, line);
		updateBlockInfo(pMem, pMem, size, BLOCK_STATUS_REALLOC);
		return pMem;
	}

	return resizeMemory(&pMem, size, file, line);

}

// Our replacement for free().
void __Free(void *pMem, char *file, int line) {
	if (pMem) {
		// Determine size of this memory block.
		size_t size = sizeOfBlock(pMem);
		assert(totalMemory >= size);

#ifdef VERBOSE
		// Report statistics.
		fprintf(stderr, "free: %s, line #%d\n", file, line);
		fprintf(stderr, " 0x%p, size: %d, total: %d\n", pMem, size, totalMemory - size);
#endif

		// Check/designate this memory as free.
		if (!setMemoryStatus(pMem)) {
			fprintf(stderr, "*** WARNING: 0x%p memory previously free'd.\n", pMem);
			return;
		}

		// Decrement total memory count.
		totalMemory -= size;

		// Check for memory access under-run.
		uint8_t *pPad = (uint8_t *)pMem - MALLOC_START_OFFSET;
		for (int i = 0; i < MALLOC_PADDING_LENGTH; i++)
			if (*(pPad + i) != _cleanLandFill)
				fprintf(stderr, "*** WARNING: Memory under-run detected at 0x%p.\n", pPad);

		// Check for memory access over-run.
		pPad = (uint8_t *)pMem + size;
		for (int i = 0; i < MALLOC_PADDING_LENGTH; i++)
			if (*(pPad + i) != _cleanLandFill)
				fprintf(stderr, "*** WARNING: Memory over-run detected at 0x%p.\n", pPad + i);

		// Paint memory as dead.
		memset(pMem, _deadLandFill, size);
	}
	else
		fprintf(stderr, "*** WARNING: free() received a NULL pointer: %s, line #%d\n", file, line);
}

// Our replacement for exit().
void __Exit(int const status) {
	// Check all released memory.	
	checkAllocations();

	// Report if all memory released.
	if (!totalMemory)
		fputs("\nAll memory de-allocated.", stderr);

	// Pause upon exit.
	fputs("\nPress Control - C to exit.\n", stderr);
	while (getchar() != EOF);

	// Perform actual system exit.
	exit(status);
}

#endif
