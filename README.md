# memTracker version 2.2

## A simplistic malloc/free memory allocation tracker framework in C language

This program hijacks the calls to ```malloc, calloc, realloc, free``` and ```exit``` to check for memory leaks, and under/over runs.

Typical issues when programming allocated memory are leaks, buffer overflow, and/or bounds error. Memory leaks primarily occur when allocated memory is not properly released when it is no longer required. Additionally, accessing memory after it has been released can cause unpredictable program crashes and errors. Bounds errors occur when a program attempts to access memory outside the limits of the amount allocated. This usually happens as an “over-run”, but could just as easily occur as an “under-run” also. These errors have very serious security consequences.
 
1. In this version, when one allocates memory via ```malloc, calloc or realloc```, the code adds a buffer above and below the requested actual allocated memory. The memTracker program then “paints” the memory with a specific byte value (```0xCC```). Therefore any writing to this area will become obvious. Upon releasing the memory via a call to the ```free or realloc``` functions, the code makes an extra check of the painted memory to identify any inappropriate access. It will alert you if it discovers any “unpainted” memory. At this point, the memory is not actually released.
 
2. After the above under/over-run checks, the memory is *not* actually released. It is again “painted” with a different value (```0xDD```) to highlight any subsequent invalid access attempts.  When the program calls ```exit```, the memory is checked one last time for invalid access. At this point the memory is finally released. 

3. Information on each block of allocated memory is kept in a linked-list structure entitled ```BLOCKINFO```.

4. The memTrack.h file includes a ```VERBOSE_OUTPUT``` define option for turning on/off additional debug printing.

I’ve attached all of the necessary files below including a basic test program which demonstrates the use of the tracker.

To use this version (currently only tested with MSVC), simply include ```memTrack.h, memTrack.c```, and ```memTracker.h``` files in your project, and add the following line to your program:
 
```#include "memTracker.h".```
 
As before, it only compiles in the debug version of your program.
