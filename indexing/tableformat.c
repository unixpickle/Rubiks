#define _LARGEFILE64_SOURCE 1
#define D_FILE_OFFSET_BITS 64
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define kCacheTableBufferSize 65536


static int indexCount = 0;
static int moveMax = 0;

static unsigned char * cacheTable;
static long long cacheTableAlloc;
static long long cacheTableCount;
static int cacheEntrySize;

void populate_cachetable(FILE * fp);
void cachetable_add(const unsigned char * indices, unsigned char moves);
int cachetable_compare(const unsigned char * entry1, const unsigned char * entry2);

int main(int argc, const char * argv[]) {
	if (argc != 6) {
		fprintf(stderr, "Converts indexer output to Alex Nichol Cube Index files.\n");
		fprintf(stderr, "Usage: %s <index count> <actual max> <move max> <source> <destination>\n",
			   argv[0]);
		return 1;
	}
	indexCount = atoi(argv[1]);
    int actualMaximum = atoi(argv[2]);
	moveMax = atoi(argv[3]);
	const char * source = argv[4];
	const char * dest = argv[5];

	cacheEntrySize = 1 + indexCount / 2 + indexCount % 2;

	cacheTableCount = 0;
	cacheTableAlloc = kCacheTableBufferSize;
	cacheTable = (unsigned char *)malloc(cacheTableAlloc * cacheEntrySize);

	FILE * input = fopen(source, "r");
	if (!input) {
		fprintf(stderr, "error: failed to open input file: %s.\n", source);
		return 1;
	}
	FILE * output = fopen(dest, "w");
	if (!output) {
		fclose(output);
		fprintf(stderr, "error: failed to open  output file.\n");
		return 1;
	}
	populate_cachetable(input);
	fclose(input);
    fprintf(output, "ANCI");
    fwrite(&actualMaximum, 4, 1, output);
    fwrite(&indexCount, 4, 1, output);
	fwrite(cacheTable, cacheEntrySize, cacheTableCount, output);
	fclose(output);
	return 0;
}

void populate_cachetable(FILE * fp) {
	fseek(fp, 0, SEEK_END);
    long long totalSize = ftello(fp);
    printf("file total size %lld\n", totalSize);
    int entrySize = indexCount + moveMax + 2;
    unsigned char * bufferData = (char *)malloc(entrySize * 1000);
    long long offset = 0;
	int addedCount = 0;
    while (offset < totalSize) {
        fseek(fp, offset, SEEK_SET);
        int buffCount = 1000; // the buff dude has a stick
        if (entrySize * buffCount + offset > totalSize) {
            buffCount = (totalSize - offset) / entrySize;
        }
        fread(bufferData, entrySize, buffCount, fp);
        addedCount += buffCount;
        if (addedCount % 1000000 == 0) {
            printf("ordered %d entries\n", addedCount);
        }
        // check if this buffer contains our object of interest
        int i;
        for (i = 0; i < buffCount; i++) {
            unsigned char * object = &bufferData[i * entrySize];
			cachetable_add(object, object[entrySize - 2]);
        }
        offset += buffCount * entrySize;
    }
    free(bufferData);
	printf("ordered a total of %d entries.\n", addedCount);
    return;
}

void cachetable_add(const unsigned char * indices, unsigned char moves) {
	// perform a binary search
	unsigned char entryData[cacheEntrySize];
	bzero(entryData, cacheEntrySize);
	int i;
	for (i = 0; i < indexCount; i++) {
		int shift = i % 2 == 0 ? 4 : 0;
		int index = i / 2;
		entryData[index] |= indices[i] << shift;
	}
	entryData[cacheEntrySize - 1] = moves;
	// perform a binary search to find the proper index
	int lowestIndex = -1;
	int highestIndex = cacheTableCount;
	while (highestIndex - lowestIndex > 1) {
		int testIndex = (highestIndex + lowestIndex) / 2;
		unsigned char * testObject = &cacheTable[testIndex * cacheEntrySize];
		int relativity = cachetable_compare(testObject, entryData);
		if (relativity == 1) {
			highestIndex = testIndex;
		} else if (relativity == -1) {
			lowestIndex = testIndex;
		} else {
			lowestIndex = testIndex;
			highestIndex = testIndex;
			break;
		}
	}
	int insertIndex = (highestIndex + lowestIndex) / 2;
	if (insertIndex < 0) insertIndex = 0;
	if (insertIndex >= cacheTableCount) insertIndex = cacheTableCount;
    if (insertIndex < cacheTableCount) {
	    unsigned char * closeObject = &cacheTable[insertIndex * cacheEntrySize];
	    int relativity = cachetable_compare(closeObject, entryData);
	    if (relativity == 0) {
		    printf("warning: attempted to add duplicate\n");
		    exit(0);
		    return;
	    } else if (relativity == -1) {
    		insertIndex ++;
	    }
    }
	if (cacheTableCount == cacheTableAlloc) {
		cacheTableAlloc += kCacheTableBufferSize;
		cacheTable = (unsigned char *)realloc(cacheTable,
											  cacheEntrySize * cacheTableAlloc);
	}
	int copyCount = cacheTableCount - insertIndex;
	if (copyCount > 0) {
		unsigned char * memStart = &cacheTable[insertIndex * cacheEntrySize];
		unsigned char * memDest = &cacheTable[(insertIndex+1) * cacheEntrySize];
		int memSize = copyCount * cacheEntrySize;
		memmove(memDest, memStart, memSize);
	}
	memcpy(&cacheTable[insertIndex * cacheEntrySize], entryData,
		   cacheEntrySize);
	cacheTableCount ++;
}

int cachetable_compare(const unsigned char * entry1, const unsigned char * entry2) {
	// compare each byte
	int byteCount = indexCount / 2 + (indexCount % 2);
	int i;
	for (i = 0; i < byteCount; i++) {
		if (entry1[i] > entry2[i]) return 1;
		if (entry1[i] < entry2[i]) return -1;
	}
	return 0;
}

