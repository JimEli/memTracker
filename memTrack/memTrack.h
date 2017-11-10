// Simple debug print malloc/free functions.
// Do not explicitly include this header, use memTracker.h instead.
// Version 2.1a
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef _DEBUG_MALLOC_H_
#define _DEBUG_MALLOC_H_

#ifdef _DEBUG

// Memory allocation is expanded by padding amount (equally spaced before/after actual).
#define MALLOC_PADDING 16
#define MALLOC_START   (MALLOC_PADDING / 4 - 1)

// Memory paint values.
static unsigned char _deadLandFill = 0xDD;  // Fill free objects with this value.
static unsigned char _cleanLandFill = 0xCC; // Fill new objects with this value.

// Memory statistics.
static size_t currentMemory = 0;

// Redirection function definitions.
void *_trackMalloc(size_t, char *, int);
void _trackFree(void *, char *, int);
void _trackExit(int const);

#endif
#endif
