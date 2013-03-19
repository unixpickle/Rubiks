#include "cube.h"
#include "subproblemindices.h"

int file_find_entry(const char * findData, FILE * fp, unsigned char * movesOut, unsigned char * moveCount);

int main (int argc, const char * argv[]) {
	const char * dbPath = NULL;
	if (argc == 2) {
		dbPath = argv[1];
	} else {
		fprintf(stderr, "Usage: %s <dbPath>\n", argv[0]);
		return 1;
	}
	RubiksMap * map = cube_user_input();
	if (!map) return 1;
	char cornerPieces[24];
	int i;
	printf("corner pieces: ");
	for (i = 0; i < 24; i++) {
		cornerPieces[i] = map->indices[CubeCornerIndices[i]] - 1;
		printf("%d ", cornerPieces[i]);
	}
	printf("\n");
	FILE * fp = fopen(dbPath, "r");
	unsigned char moves[11];
	unsigned char moveCount;
	if (file_find_entry(cornerPieces, fp, moves, &moveCount)) {
		printf("found solution in %d moves.\n", moveCount);
		int i;
		for (i = 0; i < moveCount; i++) {
			printf("%d ", (int)moves[i]);
		}
		printf("\n");
	} else {
		printf("No solution found! WTF did you do?\n");
	}
	fclose(fp);
	return 0;
}

int file_find_entry(const char * findData, FILE * fp, unsigned char * movesOut, unsigned char * moveCount) {
	fseek(fp, 0, SEEK_END);
	long totalSize = ftell(fp);
	int entrySize = 24 + 11 + 2;
	unsigned char * bufferData = (char *)malloc(entrySize * 1000);
	long offset = 0;
	int solvedCount = 0;
	while (offset < totalSize) {
		fseek(fp, offset, SEEK_SET);
		int buffCount = 1000; // the buff dude has a stick
		if (entrySize * buffCount + offset > totalSize) {
			buffCount = (totalSize - offset) / entrySize;
		}
		fread(bufferData, entrySize, buffCount, fp);
		solvedCount += buffCount;
		if (solvedCount % 5000000 == 0) {
			printf("tested %d\n", solvedCount);
		}
		// check if this buffer contains our object of interest
		int i;
		for (i = 0; i < buffCount; i++) {
			unsigned char * object = &bufferData[i * entrySize];
			if (memcmp(object, findData, 24) == 0) {
				memcpy(movesOut, &bufferData[i * entrySize + 24], 11);
				*moveCount = bufferData[i * entrySize + 24 + 11];
				free(bufferData);
				return 1;
			}
		}
		offset += buffCount * entrySize;
	}
	free(bufferData);
	return 0;
}

