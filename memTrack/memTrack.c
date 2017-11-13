// Simple debug print malloc/free functions.
// Version 2.2
#include "memTrack.h"

// This is only compiled in debug version.
#ifdef _DEBUG

// Pointer to head of memory blockinfo list.
static blockinfo *pbiHead = NULL;

// Designate pointer as valid.
void clearMemoryStatus(void) {
	blockinfo *pbi;

	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext)
		pbi->free = false;
}

// Desigante pointer as free.
void setMemoryStatus(void *pv) {
	blockinfo *pbi;

	assert(pv != NULL);
	pbi = getBlockInfo((uint8_t *)pv);
	pbi->free = true;
}

// Return blockinfo list element corresponding to memory pointer.
static blockinfo *getBlockInfo(uint8_t *pMem) {
	blockinfo *pbi;

	assert(pMem != NULL);
	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		uint8_t *pbStart = pbi->pb;
		uint8_t *pbEnd = pbi->pb + pbi->size - 1;
		if (pMem >= pbStart && pMem <= pbEnd)
				break;
	}
	assert(pbi != NULL);
	return (pbi);
}

// Create a new blockinfo list entry for memory pointer.
static bool createBlockInfo(uint8_t *pNewMem, size_t sizeNew) {
	assert(pNewMem != NULL && sizeNew != 0);

	blockinfo *pbi = (blockinfo *)malloc(sizeof(blockinfo));
	if (pbi != NULL) {
		pbi->pb = pNewMem;
		pbi->size = sizeNew;
		pbi->free = false;
		pbi->pbiNext = pbiHead;
		pbiHead = pbi;
	}
	return (bool)(pbi != NULL);
}

// Release blockinfo entry for the memory pointer.
static void freeBlockInfo(uint8_t *pbMemToFree) {
	blockinfo *pbi, *pbiPrev = NULL;

	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		if (pbi->pb == pbMemToFree) {
			if (pbiPrev == NULL)
				pbiHead = pbi->pbiNext;
			else
				pbiPrev->pbiNext = pbi->pbiNext;
			break;
		}
		pbiPrev = pbi;
	}
 	assert(pbi != NULL);
	memset(pbi, _deadLandFill, sizeof(blockinfo));
	free(pbi);
}

// Return size of memory block associated with pointer.
static size_t sizeOfBlock(uint8_t *pMem) {
	blockinfo *pbi = getBlockInfo(pMem);
	assert(pMem == pbi->pb);
	return (pbi->size);
}

// Check released memory for invalid access.
static void checkMemory(void) {
	blockinfo *pbi = pbiHead;

	// Walk the allocated memory blockinfo structure list.
	while (pbi != NULL) {
		blockinfo *next = pbi->pbiNext;

		if (pbi->pb != NULL) {
			// Get size of memory block.
			size_t size = pbi->size;
			// Has memory been freed?
			if (!pbi->free) 
				printf("*** WARNING: Memory not free'd at 0x%p\n", pbi->pb);
			else 
				// Check for dead memory access.
				for (unsigned int i = 0; i < size; i++)
					if (*(pbi->pb + i) != _deadLandFill)
						printf("*** WARNING: Free'd memory access detected at 0x%p\n", pbi->pb + i);

			// Free memory and blockinfo for this pointer.
			free((uint8_t *)pbi->pb - MALLOC_START_OFFSET);
			freeBlockInfo((uint8_t *)pbi->pb);
			
			pbi = next;
		}
	}
}

// Our replacement for exit().
void __Exit(int const status) {
	// Check all releases memory.	
	checkMemory();

	// Check for memory leaks and report if found.
	if (!totalMemory)
		fputs("\nAll memory de-allocated.", stdout);

	// Pause upon exit.
	fputs("\nPress Control - C to exit.\n", stdout);
	while (getchar() != EOF);
	exit(status);
}

// Our replacement for malloc().
void *__Malloc(size_t nSize, char *file, int line) {
	// Attempt to allocate requested size + padding.
	void *pMem = malloc(nSize + MALLOC_PADDING);
	if (pMem != NULL && createBlockInfo((uint8_t *)pMem + MALLOC_START_OFFSET, nSize)) {
		// Paint the memory as uninitailized.
		memset((uint8_t *)pMem, _cleanLandFill, nSize + MALLOC_PADDING);

		// Keep count of total allocations.
		totalMemory += nSize;

		// Print statistics.
		fprintf(stdout, "malloc: %s, line #%d\n", file, line);
		fprintf(stdout, " 0x%p, size: %d, total: %d\n", (uint8_t *)pMem + MALLOC_START_OFFSET, nSize, totalMemory);

		// Retun memory requested.
		return((uint8_t *)pMem + MALLOC_START_OFFSET);
	}
	return NULL;
}

// Our replacement for free().
void __Free(void *pMem, char *file, int line) {
	if (pMem) {
		size_t size = sizeOfBlock((uint8_t *)pMem);
		assert(totalMemory >= size);

		setMemoryStatus(pMem);
		// Check for memory access under-run.
		uint8_t *pPad = (uint8_t *)pMem - MALLOC_START_OFFSET;
		for (int i = 0; i < MALLOC_PADDING_LENGTH; i++)
			if (*(pPad + i) != _cleanLandFill)
				printf("*** WARNING: Memory under-run detected at 0x%p\n", pPad);
		// Check for memory access over-run.
		pPad = (uint8_t *)pMem + size;
		for (int i = 0; i < MALLOC_PADDING_LENGTH; i++)
			if (*(pPad + i) != _cleanLandFill)
				printf("*** WARNING: Memory over-run detected at 0x%p\n", pPad + i);
		// Decrement memory count.
		totalMemory -= size;

		// Report statistics.
		fprintf(stdout, "free: %s, line #%d\n", file, line);
		fprintf(stdout, " 0x%p, size: %d, total: %d\n", pMem, size, totalMemory);

		// Paint memory as dead.
		memset((uint8_t *)pMem, _deadLandFill, size);
	}
}

#endif
