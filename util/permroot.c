#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static count = 0;
static int ** allocTable;

int * parsePermutation(const char * string, int * count);
void mulitplyPermutation(int * out, const int * perm1, const int * perm2, int count);
void recursiveSearch(const int * indexes, int * perm,
                     int * soFar, int depth, int length);

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <permutation>\n", argv[0]);
        return 1;
    }

    int count, i;
    int * numbers = parsePermutation(argv[1], &count);
    allocTable = (int **)malloc(sizeof(int *) * count);
    int * counters = (int *)malloc(sizeof(int) * count);
    for (i = 0; i < count; i++) {
        counters[i] = i + 1;
        allocTable[i] = malloc(sizeof(int) * count);
    }
    int * zeros = (int *)malloc(sizeof(int) * count);
    bzero(zeros, sizeof(int) * count);
    recursiveSearch(counters, numbers, zeros, 0, count);

    free(zeros);
    free(numbers);
    free(counters);

    for (i = 0; i < count; i++) {
        free(allocTable[i]);
    }
    free(allocTable);

    return 0;
}

int * parsePermutation(const char * string, int * count) {
    char * buffer = (char *)malloc(strlen(string) + 1);
    int wordStart = 0;
    int hasWord = 0;
    int numberCount = 0;
    int * numbers = (int *)malloc(sizeof(int));
    int i;
    for (i = 0; i <= strlen(string); i++) {
        if (isspace(string[i]) || string[i] == 0) {
            if (hasWord) {
                hasWord = 0;
                numberCount++;
                numbers = (int *)realloc(numbers, sizeof(int) * (numberCount + 1));
                memcpy(buffer, &string[wordStart], i - wordStart);
                buffer[i - wordStart] = 0;
                numbers[numberCount - 1] = atoi(buffer);
            }
        } else {
            if (!hasWord) {
                hasWord = 1;
                wordStart = i;
            }
        }
    }
    free(buffer);
    *count = numberCount;
    return numbers;
}

void multiplyPermutation(int * out, const int * perm1, const int * perm2, int count) {
    int i;
    for (i = 0; i < count; i++) {
        out[i] = perm2[perm1[i] - 1];
    }
}

void recursiveSearch(const int * indexes, int * perm,
                     int * soFar, int depth, int length) {
    int i;
    if (length == depth) {
        count++;
        int * dest = (int *)malloc(sizeof(int) * length);
        multiplyPermutation(dest, soFar, soFar, length);
        if (memcmp(dest, perm, length * sizeof(int)) == 0) {
            printf("permutation:");
            for (i = 0; i < length; i++) {
                printf(" %d", soFar[i]);
            }
            printf("\n");
        }
        if (count % 65536 == 0) {
            printf("Tried %d permutations\n", count);
        }
        return;
    }
    for (i = 0; i < depth; i++) {
        int referral = soFar[i] - 1;
        if (referral < depth) {
            if (soFar[referral] != perm[i]) return;
        }
    }
    int * indexCopy = allocTable[depth];
    memcpy(indexCopy, indexes, sizeof(int) * length);
    for (i = 0; i < length; i++) {
        if (indexCopy[i] != 0) {
            int j = indexCopy[i];
            indexCopy[i] = 0;
            soFar[depth] = j;
            recursiveSearch(indexCopy, perm, soFar, depth + 1, length);
            indexCopy[i] = j;
        }
    }
}

