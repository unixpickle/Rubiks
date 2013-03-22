#include "cubeindex.h"
#include <stdio.h>

static int cachetable_compare(const unsigned char * entry1, const unsigned char * entry2, int indexCount);

cube_index_t * cube_index_load(const char * path) {
    FILE * fp = fopen(path, "r");
    if (!fp) return NULL;
    uint32_t moveMax = 0;
    uint32_t indexCount = 0;
    fseek(fp, 0, SEEK_END);
    long long size = ftell(fp);
    if (size < 12) {
        fclose(fp);
        return NULL;
    }
    fseek(fp, 4, SEEK_SET);
    fread(&moveMax, 4, 1, fp);
    fread(&indexCount, 4, 1, fp);
    int entrySize = indexCount / 2 + (indexCount % 2) + 1;
    int entryCount = (size - 12) / entrySize;
    cube_index_t * cubeIndex = (cube_index_t *)malloc(sizeof(cube_index_t));
    cubeIndex->configurationData = (unsigned char *)malloc(size - 12);
    cubeIndex->entryCount = entryCount;
    cubeIndex->entrySize = entrySize;
    cubeIndex->defaultMax = moveMax;
    cubeIndex->indexCount = indexCount;
    fread(cubeIndex->configurationData, 1, size - 12, fp);
    fclose(fp);
    return cubeIndex;
}

void cube_index_free(cube_index_t * index) {
    free(index->configurationData);
    free(index);
}

int cube_index_lookup_moves(cube_index_t * index, RubiksMap * map, const unsigned char * significantIndices) {
    int entrySize = index->entrySize;
    unsigned char * tableData = index->configurationData;
    unsigned char lookupData[27];
    bzero(lookupData, 27);
    int i;
    for (i = 0; i < index->indexCount; i++) {
        int shift = i % 2 == 0 ? 4 : 0;
        lookupData[i / 2] |= (map->indices[significantIndices[i]] - 1) << shift;
    }
    int lowestIndex = -1;
    int highestIndex = index->entryCount;
    while (highestIndex - lowestIndex > 1) {
        int testIndex = (highestIndex + lowestIndex) / 2;
        unsigned char * testObject = &tableData[testIndex * entrySize];
        int relativity = cachetable_compare(testObject, lookupData,
                                            index->indexCount);
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
    int foundIndex = (highestIndex + lowestIndex) / 2;
    if (foundIndex < 0) return index->defaultMax;
    if (foundIndex >= index->entryCount) return index->defaultMax;
    unsigned char * foundObject = &tableData[foundIndex * entrySize];
    if (cachetable_compare(lookupData, foundObject, index->indexCount) == 0) {
        return foundObject[entrySize - 1];
    }
    return index->defaultMax;
}

static int cachetable_compare(const unsigned char * entry1, const unsigned char * entry2, int indexCount) {
    // compare each byte
    int byteCount = indexCount / 2 + (indexCount % 2);
    int i;
    for (i = 0; i < byteCount; i++) {
        if (entry1[i] > entry2[i]) return 1;
        if (entry1[i] < entry2[i]) return -1;
    }
    return 0;
}

