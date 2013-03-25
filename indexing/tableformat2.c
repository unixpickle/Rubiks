/**
 * Converts a .dat indexing output to an "Alex Nichol Cube Index 2" file.
 * The ANCI2 format uses the piecewise storage method instead of
 * storing individual color indices. It requires a more detailed
 * header.
 */

#define _LARGEFILE64_SOURCE 1
#define D_FILE_OFFSET_BITS 64
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "subproblemindices.h"
#include "stickerindices.h"
#include "symmetry.h"

#define kCacheTableBufferSize 65536

typedef enum {
	TableIndexTypeCorners = 4,
	TableIndexTypeEdgeFront = 1,
	TableIndexTypeEdgeBack = 2,
	TableIndexTypeEdgeAll = 3
} TableIndexType;

static long long dbEntrySize = 0;
static long long dbFileSize = 0;
static long long dbMovesCount = 0;
static long long dbIndexCount = 0;
static unsigned char dbSignificantIndices[54];

static long long entrySize = 0;
static long long entryCount = 0;
static TableIndexType indexType = 0;
static unsigned char * outputData;
static long long encodedCount;

void addAllDbEntries(FILE * input);
void addDbEntry(const unsigned char * dbEntry);
void convertEntry(unsigned char * output, const unsigned char * dbInput);
int compareEntries(const unsigned char * e1, const unsigned char * e2);

void convertCorners(unsigned char * cornersOut, const unsigned char * dbData);
void convertEdges(unsigned char * edgesOut, const unsigned char * dbData, int edgeStart, int edgeCount);

int main(int argc, const char * argv[]) {
	if (argc != 6) {
		fprintf(stderr, "** Convert indexer output to ANCI2 files. **\n");
		fprintf(stderr, "Usage: %s <actual max> <move max> <index type> <source> <destination>\n",
			   argv[0]);
		fprintf(stderr, "Index types: \n\
 corners - the corners of the cube\n\
 edgefront - the first 6 edges of the cube\n\
 edgeback - the last 6 edges of the cube\n\
 edgeall - all 12 edges of the cube\n\n");
		return 1;
	}
	int actualMaximum = atoi(argv[1]);
	dbMovesCount = atoi(argv[2]);
	const char * indexTypeStr = argv[3];
	const char * source = argv[4];
	const char * dest = argv[5];

	if (strcmp(indexTypeStr, "corners") == 0) {
		entrySize = 9;
		indexType = TableIndexTypeCorners;
		dbIndexCount = 24;
		memcpy(dbSignificantIndices, CubeCornerIndices, 24);
	} else if (strcmp(indexTypeStr, "edgefront") == 0) {
		entrySize = 7;
		dbIndexCount = 12;
		indexType = TableIndexTypeEdgeFront;
		memcpy(dbSignificantIndices, CubeFrontIndices, 12);
	} else if (strcmp(indexTypeStr, "edgeback") == 0) {
		entrySize = 7;
		dbIndexCount = 12;
		indexType = TableIndexTypeEdgeBack;
		memcpy(dbSignificantIndices, CubeBackIndices, 12);
	} else if (strcmp(indexTypeStr, "edgeall") == 0) {
		entrySize = 13;
		dbIndexCount = 24;
		indexType = TableIndexTypeEdgeAll;
		memcpy(dbSignificantIndices, CubeBackIndices, 12);
		memcpy(&dbSignificantIndices[12], CubeFrontIndices, 12);
	} else {
		fprintf(stderr, "error: unknown indexType %s\n", indexTypeStr);
		return 1;
	}

	dbEntrySize = dbIndexCount + dbMovesCount + 2;
	
	FILE * inputFile = fopen(source, "r");
	if (!inputFile) {
		fprintf(stderr, "error: unable to read input file.\n");
		return 1;
	}
	
	// get the file size and entry count
	fseek(inputFile, 0, SEEK_END);
	dbFileSize = ftello(inputFile);
	entryCount = dbFileSize / dbEntrySize;
	fseek(inputFile, 0, SEEK_SET);
	
	outputData = (unsigned char *)malloc(entryCount * entrySize);
	printf("adding %lld entries (%lld bytes)\n", entryCount, entryCount * entrySize);
	addAllDbEntries(inputFile);
	fclose(inputFile);
	
	FILE * outputFile = fopen(dest, "w");
	// TODO: write headers here
    unsigned char theMoveMax = actualMaximum;
    unsigned char type = indexType;
    fprintf(outputFile, "ANC2");
    fwrite(&type, 1, 1, outputFile);
    fwrite(&theMoveMax, 1, 1, outputFile);
	fwrite(outputData, entrySize, entryCount, outputFile);
	fclose(outputFile);
	free(outputData);

    printf("encoded a total of %lld entries\n", encodedCount);

	return 0;
}

void addAllDbEntries(FILE * input) {
	unsigned char * buffer = (unsigned char *)malloc(dbEntrySize * kCacheTableBufferSize);
	long long entriesDone = 0;
	while (entriesDone < entryCount) {
		long long readEntries = kCacheTableBufferSize;
		if (entriesDone + readEntries > entryCount) {
			readEntries = entryCount - entriesDone;
		}
		fread(buffer, dbEntrySize, readEntries, input);
		int i;
		for (i = 0; i < readEntries; i++) {
			const unsigned char * entry = &buffer[i * dbEntrySize];
			addDbEntry(entry);
		}
        entriesDone += readEntries;
        printf("done %lld entries\n", entriesDone);
	}
}

void addDbEntry(const unsigned char * dbEntry) {
	char insertionObject[54];
	convertEntry(insertionObject, dbEntry);

	// perform a binary search to find the insertion index
	int lowestIndex = -1;
	int highestIndex = encodedCount;
	while (highestIndex - lowestIndex > 1) {
		int testIndex = (highestIndex + lowestIndex) / 2;
		const unsigned char * testObject = &outputData[testIndex * entrySize];
		int relativity = compareEntries(testObject, insertionObject);
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
	if (insertIndex >= encodedCount) insertIndex = encodedCount;
    if (insertIndex < encodedCount) {
	    unsigned char * closeObject = &outputData[insertIndex * entrySize];
	    int relativity = compareEntries(closeObject, insertionObject);
	    if (relativity == 0) {
		    fprintf(stderr, "warning: attempted to add duplicate\n");
		    exit(0);
	    } else if (relativity == -1) {
    		insertIndex ++;
	    }
    }
	int copyCount = encodedCount - insertIndex;
	if (copyCount > 0) {
		unsigned char * memStart = &outputData[insertIndex * entrySize];
		unsigned char * memDest = &outputData[(insertIndex + 1) * entrySize];
		int memSize = copyCount * entrySize;
		memmove(memDest, memStart, memSize);
	}
	memcpy(&outputData[insertIndex * entrySize], insertionObject, entrySize);
	encodedCount++;
}

void convertEntry(unsigned char * output, const unsigned char * dbInput) {
	output[entrySize - 1] = dbInput[dbEntrySize - 2];
	if (indexType == TableIndexTypeCorners) {
		convertCorners(output, dbInput);
	} else if (indexType == TableIndexTypeEdgeFront) {
		convertEdges(output, dbInput, 0, 6);
	} else if (indexType == TableIndexTypeEdgeBack) {
		convertEdges(output, dbInput, 6, 6);
	} else if (indexType == TableIndexTypeEdgeAll) {
		convertEdges(output, dbInput, 0, 12);
	}
}

int compareEntries(const unsigned char * e1, const unsigned char * e2) {
	int i;
	for (i = 0; i < entrySize; i++) {
		if (e1[i] > e2[i]) return 1;
		else if (e1[i] < e2[i]) return -1;
	}
	return 0;
}

/////////////////
// Conversions //
/////////////////

void convertCorners(unsigned char * cornersOut, const unsigned char * dbData) {
	int i, j, k;
	for (i = 0; i < 8; i++) {
		unsigned char realColors[3];
		for (j = 0; j < 3; j++) {
			int cornerIndex = CornerIndices[i][j];
			int dataIndex = -1;
			for (k = 0; k < dbIndexCount; k++) {
				if (dbSignificantIndices[k] == cornerIndex) {
					dataIndex = k;
					break;
                }
			}
			if (dataIndex < 0) {
				fprintf(stderr, "error: couldn't look up corner index %d\n", cornerIndex);
				exit(0);
			}
			realColors[j] = dbData[dataIndex] + 1;
		}
		int realIndex = -1, symmetry = -1;
		for (j = 0; j < 8; j++) {
			symmetry = symmetry_operation_find(CornerPieces[j], realColors);
			if (symmetry >= 0) {
				realIndex = j;
				break;
			}
		}
		if (realIndex < 0) {
            fprintf(stderr, "error: failed to match color configuration.\n");
            exit(0);
		}
		cornersOut[i] = realIndex | (symmetry << 4);
	}
}

void convertEdges(unsigned char * edgesOut, const unsigned char * dbData, int edgeStart, int edgeCount) {
	int i, j, k;
	for (i = edgeStart; i < edgeStart + edgeCount; i++) {
		unsigned char realColors[3];
		for (j = 0; j < 3; j++) {
			int cornerIndex = EdgeIndices[i][j];
			int dataIndex = -1;
			for (k = 0; k < dbIndexCount; k++) {
				if (dbSignificantIndices[k] == cornerIndex) {
					dataIndex = k;
					break;
				}
			}
			if (dataIndex < 0) {
				fprintf(stderr, "error: couldn't look up corner index\n");
				exit(0);
			}
			realColors[j] = dbData[dataIndex] + 1;
		}
		int realIndex = -1, symmetry = -1;
		for (j = 0; j < 12; j++) {
			symmetry = symmetry_operation_find(EdgePieces[j], realColors);
			if (symmetry >= 0) {
				realIndex = j;
				break;
			}
		}
		if (realIndex < 0) {
            fprintf(stderr, "error: failed to match color configuration.\n");
            exit(0);
		}
		edgesOut[i] = realIndex | (symmetry << 4);
	}
}
