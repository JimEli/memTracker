// Simple debug print malloc/free functions.
// Version 2.1
#include "memTrack.h"

// This is only compiled in debug version.
#ifdef _DEBUG

// Our replacement for malloc().
void *_trackMalloc(size_t nSize, char *file, int line) {
	// Attempt to allocate requested size + padding.
	void *pMem = malloc(sizeof(size_t) + nSize + MALLOC_PADDING);
	if (pMem) {
		size_t *pSize = (size_t *)pMem;
		// Save the requested allocation size.
		memcpy(pSize, &nSize, sizeof(nSize));

		// Paint the memory as uninitailized.
		memset(pSize + 1, _cleanLandFill, nSize + MALLOC_PADDING);

		// Keep count of total allocations.
		currentMemory += nSize;

		// Print statistics.
		fprintf(stdout, "malloc: %s, line #%d\n", file, line);
		fprintf(stdout, " 0x%p, size: %d, total: %d\n", pSize, nSize, currentMemory);

		// Retun memory requested.
		return(pSize + MALLOC_START);
	}
	return NULL;
}

// Our replacement for free().
void _trackFree(void *pMem, char *file, int line) {
	if (pMem) {
		// Get original location and size of this block.
		size_t *pSize = (size_t *)pMem;
		pSize -= MALLOC_START;
		assert(currentMemory >= *pSize);

		// Check for memory access under-runs.
		unsigned char *pPad = (unsigned char *)(pSize + 1);
		for (int i = 0; i < MALLOC_PADDING / 2; i++)
			if (*(pPad + i) != _cleanLandFill)
				printf("*** WARNING: Memory under-run detected at 0x%p\n", pPad);
		// Check for memory access over-runs.
		pPad = (unsigned char *)pMem + *pSize;
		for (int i = 0; i < MALLOC_PADDING / 2; i++)
			if (*(pPad + i) != _cleanLandFill)
				printf("*** WARNING: Memory over-run detected at 0x%p\n", pPad + i);
		// Decrement memory count.
		currentMemory -= *pSize;

		// Report statistics.
		fprintf(stdout, "free: %s, line #%d\n", file, line);
		fprintf(stdout, " 0x%p, size: %d, total: %d\n", pSize, *pSize, currentMemory);

		// Paint memory as dead.
		memset(pSize, _deadLandFill, *pSize + MALLOC_PADDING + sizeof(size_t));

		// Free memory.
		free(pSize);
	}
}

// Our replacement for exit().
void _trackExit(int const status) {
	// Check for memory leaks and report if found.
	if (currentMemory)
		fprintf(stdout, "\n*** WARNING: (%d) allocated memory not freed!", currentMemory);
	else
		fputs("\nAll memory de-allocated.", stdout);

	// Pause upon exit.
	fputs("\nPress Control - C to exit.\n", stdout);
	while (getchar() != EOF);
	exit(status);
}

#endif
