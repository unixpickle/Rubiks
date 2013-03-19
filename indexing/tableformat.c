#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define kCacheTableBufferSize 65536

static int indexCount = 0;
static int moveMax = 0;

static unsigned char * cacheTable;
static long cacheTableAlloc;
static long cacheTableCount;
static int cacheEntrySize;

void cachetable_add(const unsigned char * indices, unsigned char moves);
int cachetable_compare(const unsigned char * entry1, const unsigned char * entry2);

int main(int argc, const char * argv[]) {
	if (argc != 5) {
		fprintf(stderr, "Converts indexer output to Alex Nichol Cube Index files.\n");
		fprintf(stderr, "Usage: %s <index count> <move max> <source> <destination>\n",
			   argv[0]);
		return 1;
	}
	indexCount = atoi(argv[1]);
	moveMax = atoi(argv[2]);
	const char * source = argv[3];
	const char * dest = argv[4];
	
	int cacheEntrySize = 1 + indexCount / 2 + indexCount % 2;
	return 0;
}

void cachetable_add(const unsigned char * indices, unsigned char moves) {
	// perform a binary search
	unsigned char entryData[cacheEntrySize];
	bzero(entryData, cacheEntrySize);
	int i;
	for (i = 0; i < indexCount; i++) {
		int shift = indexCount % 2 == 0 ? 4 : 0;
		int index = i / 2;
		entryData[i] |= indices[i] << shift;
	}
	entryData[cacheEntrySize - 1] = moves;
	// perform a binary search to find the proper index
	int lowestIndex = -1;
	int highestIndex = cacheTableCount;
	while (highestIndex - lowestIndex > 1) {
		int testIndex = (highestIndex + lowestIndex) / 2;
		unsigned char * testObject = &entryData[testIndex * cacheEntrySize];
		int relativity = cachetable_compare(testObject, entryData);
		if (relativity > 1) {
		}
	}
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

