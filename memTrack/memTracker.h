// Include this header file in your program to use memTracker.
// Version 2.1

#ifdef _DEBUG

// Debug memory allocation routines.
#include "memTrack.h"

// Macros defined after our replacement functions.
#define malloc(s) _trackMalloc(s, __FILE__, __LINE__)
#define free(p) _trackFree(p, __FILE__, __LINE__)
#define exit(s) _trackExit(s)

#endif