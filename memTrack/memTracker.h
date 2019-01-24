/*************************************************************************
* Title: memTracker.
* File: memTracker.h
* Author: James Eli
* Date: 11/13/2017
*
* This file is the include file for memTracker, a simple memory allocation 
* and tracking program.
*
* Notes:
*  (1) Compiled with MS Visual Studio 2017 Community (v141), using C
*      language options.
*  (2) Does not compile in release version.
*  (3) Include this header file in your program to use memTracker.
*  (4) Define VERBOSE_OUTPUT to recieve malloc/free debug prints.
*  (5) Released into the public domain.
*************************************************************************
* Change Log:
*   11/13/2017: Initial release. JME
*************************************************************************/
#ifdef _DEBUG

// Debug memory allocation routines.
#include "memTrack.h"

// Macros defined after our replacement functions.
#define malloc(s)     __Malloc(s, __FILE__, __LINE__)
#define calloc(n, s)  __Calloc(n, s, __FILE__, __LINE__)
#define realloc(p, s) __Realloc(p, s, __FILE__, __LINE__)
#define free(p)       __Free(p, __FILE__, __LINE__)
#define exit(s)       __Exit(s)

#endif

