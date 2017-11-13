// Include this header file in your program to use memTracker.
// Version 2.2

#ifdef _DEBUG

// Debug memory allocation routines.
#include "memTrack.h"

// Macros defined after our replacement functions.
#define malloc(s) __Malloc(s, __FILE__, __LINE__)
#define free(p) __Free(p, __FILE__, __LINE__)
#define exit(s) __Exit(s)

#endif
