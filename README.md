# memTracker version 2.0

## A simplistic malloc/free memory allocation tracker framework in C language

This program hijacks the calls to ```malloc, free``` and ```exit``` to check for memory leaks, and under/over runs.
 
A typical failing in programming with allocated memory is buffer overflow, or bounds error. This occurs when a program attempts to access memory outside the limits of the amount allocated. This usually happens as an “over-run”, but could just as easily occur as an “under-run” also. It has very serious security consequences.
 
1. In this version, when one allocates memory via ```malloc```, the code adds a buffer above and below the requested amount. It then “paints” the memory with a specific byte value (```0xCC```), so any writing to this area will become obvious. Upon releasing the memory via a call to the ```free``` function, the code makes an extra check of the painted memory to identify any inappropriate access. It will alert you to discovery of any “unpainted” memory.
 
2. Additionally, when the memory is released via ```free```, it is “painted” again with a slightly different value (```0xDD```) to highlight any subsequent invalid access.  However, this access must be self-discovered through the use of a debugger. The next feature to be added will be an automatic check of the released memory. However, this will necessitate a major re-write of the code.

I’ve attached all of the necessary files below including a basic test program which demonstrates how to use the tracker.
 
To use this version (currently only with MSVC), simply include ```memTrack.h, memTrack.c```, and ```memTracker.h``` in your project files, and add the following line to your program:
 
```#include "memTracker.h".```
 
As before, it only compiles in the debug version of your program.
