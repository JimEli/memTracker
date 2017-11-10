//block.c
#ifdef _DEBUG

#include "block.h"
#include "memTrack.h"

#define fPtrEqual(pLeft, pRight)  ((pLeft) == (pRight))
#define fPtrLessEq(pLeft, pRight) ((pLeft) <= (pRight))
#define fPtrGrtrEq(pLeft, pRight) ((pLeft) >= (pRight))

static blockinfo *pbiHead = NULL;

static blockinfo *pbiGetBlockInfo(uint8_t *pb) {
	blockinfo *pbi;

	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		uint8_t *pbStart = pbi->pb;
		uint8_t *pbEnd = pbi->pb + pbi->size - 1;
		if (fPtrGrtrEq(pb, pbStart) && fPtrLessEq(pb, pbEnd))
			break;
	}
	assert(pbi != NULL);
	return (pbi);
}

bool fCreateBlockInfo(uint8_t *pbNew, size_t sizeNew) {
	blockinfo *pbi;

	assert(pbNew != NULL && sizeNew != 0);
	pbi = (blockinfo *)malloc(sizeof(blockinfo));
	if (pbi != NULL) {
		pbi->pb = pbNew;
		pbi->size = sizeNew;
		pbi->pbiNext = pbiHead;
		pbiHead = pbi;
	}
	return (bool)(pbi != NULL);
}

void FreeBlockInfo(uint8_t *pbToFree) {
	blockinfo *pbi, *pbiPrev;

	pbiPrev = NULL;
	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		if (fPtrEqual(pbi->pb, pbToFree)) {
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

void UpdateBlockInfo(uint8_t *pbOld, uint8_t *pbNew, size_t sizeNew) {
	blockinfo *pbi;

	assert(pbNew != NULL && sizeNew != 0);
	pbi = pbiGetBlockInfo(pbOld);
	assert(pbOld == pbi->pb);
	pbi->pb = pbNew;
	pbi->size = sizeNew;
}

size_t sizeofBlock(uint8_t *pb) {
	blockinfo *pbi;

	pbi = pbiGetBlockInfo(pb);
	assert(pb == pbi->pb);
	return (pbi->size);
}

void ClearMemoryRefs(void) {
	blockinfo *pbi;

	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext)
		pbi->fReferenced = false;
}

void NoteMemoryRef(void *pv) {
	blockinfo *pbi;

	pbi = pbiGetBlockInfo((uint8_t *)pv);
	pbi->fReferenced = true;
}

void CheckMemoryRefs(void) {
	blockinfo *pbi;

	for (pbi = pbiHead; pbi != NULL; pbi = pbi->pbiNext) {
		assert(pbi->pb != NULL && pbi->size != 0);
		assert(pbi->fReferenced);
	}
}

bool fValidPointer(void *pv, size_t size) {
	blockinfo *pbi = NULL;
	uint8_t *pb = (uint8_t *)pv;

	assert(pv != NULL && size != 0);
	pbi = pbiGetBlockInfo(pb);
	assert(fPtrLessEq(pb + size, pbi->pb + pbi->size));
	return (true);
}

bool fNewMemory(void **ppv, size_t size) {
	uint8_t **ppb = (uint8_t **)ppv;

	assert(ppv != NULL && size != 0);
	*ppb = (uint8_t *)malloc(size);
#ifdef _DEBUG
	{
		if (*ppb != NULL) {
			memset(*ppb, _cleanLandFill, size);
			if (!fCreateBlockInfo(*ppb, size)) {
				free(*ppb);
				*ppb = NULL;
			}
		}
	}
#endif
	return (bool)(*ppb != NULL);
}

void fFreeMemory(void *pv) {
#ifdef _DEBUG
	{
		memset(pv, _deadLandFill, sizeofBlock(pv));
		FreeBlockInfo(pv);
	}
#endif
	free(pv);
}

bool fResizeMemory(void **ppv, size_t sizeNew) {
	uint8_t **ppb = (uint8_t **)ppv;
	uint8_t *pbNew;
#ifdef _DEBUG
	size_t sizeOld;
#endif
#ifdef _DEBUG
	sizeOld = sizeofBlock(*ppb);
	if (sizeNew < sizeOld)
		memset((*ppb) + sizeNew, _deadLandFill, sizeOld - sizeNew);
	else if (sizeNew > sizeOld) {
		uint8_t *pbForceNew;
		if (fNewMemory(&pbForceNew, sizeNew)) {
			memcpy(pbForceNew, *ppb, sizeOld);
			fFreeMemory(*ppb);
			*ppb = pbForceNew;
		}
	}
#endif
	pbNew = (uint8_t *)realloc(*ppv, sizeNew);
	if (pbNew != NULL) {
#ifdef _DEBUG
		UpdateBlockInfo(*ppb, pbNew, sizeNew);
		if (sizeNew > sizeOld)
			memset(pbNew + sizeOld, _cleanLandFill, sizeNew - sizeOld);
	}
#endif
	*ppb = pbNew;
	return (bool)(pbNew != NULL);
}

#endif

