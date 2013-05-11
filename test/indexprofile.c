#include "index.h"
#include <stdio.h>

void performProfile(ShardNode * node, int dataSize);

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
    const char * indexTypeName = "unknown";
    switch (type) {
        case IndexTypeCorners:
            indexTypeName = "corners";
            break;
        case IndexTypeEdgeFront:
            indexTypeName = "edgefront";
            break;
        case IndexTypeEdgeBack:
            indexTypeName = "edgeback";
            break;
        case IndexTypeEdgeAll:
            indexTypeName = "edgeall";
            break;
        case IndexTypeEO:
            indexTypeName == "eo";
            break;
        default:
            break;
    }
    printf("Max Moves: %d\nIndexType: %s (%d)\n", maxMoves, indexTypeName, type);
    printf("Performing profile...\n");
    performProfile(index, index_type_data_size(type) + 1);
    shard_node_free(index);
    return 0;
}

void performProfile(ShardNode * node, int dataSize) {
    int instanceCount[256];
    bzero(instanceCount, sizeof(instanceCount));
    int i;
    for (i = 0; i < node->nodeDataSize; i++) {
        unsigned char * data = &node->nodeData[i * dataSize];
        instanceCount[data[dataSize - 1]] ++;
    }
    int total = 0;
    for (i = 0; i < 256; i++) {
        if (instanceCount[i] > 0) {
            printf("%d - %d\n", i, instanceCount[i]);
            total += instanceCount[i];
        }
    }
    printf("total number of %d entries.\n", total);
}
