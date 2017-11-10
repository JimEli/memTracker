//block.h
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#ifdef _DEBUG

typedef struct BLOCKINFO {
	struct BLOCKINFO *pbiNext;
	uint8_t *pb;
	size_t size;
	bool fReferenced;
} blockinfo;

bool fCreateBlockInfo(uint8_t *pbNew, size_t sizeNew);
void FreeBlockInfo(uint8_t *pbToFree);
void UpdateBlockInfo(uint8_t *pbOld, uint8_t *pbNew, size_t sizeNew);
size_t sizeofBlock(uint8_t *pb);
void ClearMemoryRefs(void);
void NoteMemoryRef(void *pv);
void CheckMemoryRefs(void);
bool fValidPointer(void *pv, size_t size);

#endif
