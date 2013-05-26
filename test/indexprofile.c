#include "indexing/index.h"
#include <stdio.h>

void performProfile(ShardNode * node, long long dataSize);

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.anc2>\n", argv[0]);
        return 1;
    }
    const char * file = argv[1];
    IndexType type;
    unsigned char maxMoves;
    ShardNode * index = index_file_read(file, &type, &maxMoves);
    if (!index) {
        fprintf(stderr, "error: failed to load file.\n");
        return 1;
    }
    const char * indexTypeName = index_type_to_string(type);
    printf("Max Moves: %d\nIndexType: %s (%d)\n", maxMoves, indexTypeName, type);
    printf("Performing profile...\n");
    performProfile(index, index_type_data_size(type) + 1);
    shard_node_free(index);
    return 0;
}

void performProfile(ShardNode * node, long long dataSize) {
    printf("Enumerating %llu nodes\n", node->nodeDataSize);
    int instanceCount[256];
    bzero(instanceCount, sizeof(instanceCount));
    long long i;
    for (i = 0; i < node->nodeDataSize; i++) {
        unsigned char * data = &node->nodeData[i * dataSize];
        instanceCount[data[dataSize - 1]] ++;
    }
    int total = 0;
    for (i = 0; i < 256; i++) {
        if (instanceCount[i] > 0) {
            printf("%lld - %d\n", i, instanceCount[i]);
            total += instanceCount[i];
        }
    }
    printf("total number of %d entries.\n", total);
}
