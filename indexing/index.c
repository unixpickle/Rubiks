#include "index.h"

#define kBufferSize (1 << 24)

static const char * SubproblemNames[] = {"unknown", "edgefront", "edgeback", "edgeall",
    "corners", "eo",
    "block0", "block1", "block2", "block3", "block4",
    "block5", "block6", "block7", "cross1", "cross2",
    "cross3", "cross4", "cross5", "cross6",
    "R2F2U", "edgefirst", "edgesecond", "edgeperm"};

static void _index_recursive_write(FILE * output, ShardNode * node,
        unsigned char * data, int depth,
        int entrySize, int writeSize);

static void _index_copy_edge_data(RubiksMap * map, IndexType it, unsigned char * data);
static void _index_copy_block_data(RubiksMap * map, IndexType it, unsigned char * data);
static void _index_copy_cross_data(RubiksMap * map, IndexType it, unsigned char * data);
static void _index_copy_r2f2u_data(RubiksMap * map, IndexType it, unsigned char * data);
static void _index_copy_edge_permutation_data(RubiksMap * map, unsigned char * data);

IndexType index_type_from_string(const char * str) {
    int i;
    for (i = 0; i < 24; i++) {
        if (strcmp(str, SubproblemNames[i]) == 0) {
            return i;
        }
    }
    return IndexTypeUnknown;
}

const char * index_type_to_string(IndexType type) {
    return SubproblemNames[type];
}

int index_type_data_size(IndexType it) {
    if (it == IndexTypeEdgeAll) return 12;
    if (it == IndexTypeCorners) return 8;
    if (it == IndexTypeEO) return 2;
    if (it >= 6 && it <= 13) return 4;
    if (it >= 14 && it <= 19) return 4;
    if (it == IndexTypeR2F2UGroup) return 6;
    return 6;
}

void index_type_copy_data(IndexType it, unsigned char * data, RubiksMap * map) {
    if (it == IndexTypeCorners) {
        memcpy(data, map->pieces, 8);
    } else if (it == IndexTypeEdgeFront) {
        memcpy(data, &map->pieces[8], 6);
    } else if (it == IndexTypeEdgeBack) { 
        memcpy(data, &map->pieces[14], 6);
    } else if (it == IndexTypeEdgeAll) {
        memcpy(data, &map->pieces[8], 12);
    } else if (it == IndexTypeEO) {
        uint16_t buffer = rubiks_map_edge_orientations(map);
        data[0] = buffer & 0xff;
        data[1] = (buffer >> 8) & 0xff;
    } else if (it >= 6 && it <= 13) {
        _index_copy_block_data(map, it, data);
    } else if (it >= 14 && it <= 19) {
        _index_copy_cross_data(map, it, data);
    } else if (it == IndexTypeR2F2UGroup) {
        _index_copy_r2f2u_data(map, it, data);
    } else if (it == IndexTypeEdgeFirst || it == IndexTypeEdgeSecond) {
        _index_copy_edge_data(map, it, data);
    } else if (it == IndexTypeEdgePermutation) {
        _index_copy_edge_permutation_data(map, data);
    }
}

void index_file_write(IndexType it,
                      int writeSize,
                      int dataSize,
                      int maxDepth,
                      ShardNode * baseNode,
                      FILE * output) {
    unsigned char theMoveMax = maxDepth;
    unsigned char type = it;
    fprintf(output, "ANC2");
    fwrite(&type, 1, 1, output);
    fwrite(&theMoveMax, 1, 1, output);
    char dataBuffer[64];
    _index_recursive_write(output, baseNode,
            dataBuffer, 0, dataSize,
            writeSize);
}

ShardNode * index_file_read(const char * path,
        IndexType * t,
        unsigned char * moveMax) {
    // read the file header
    FILE * fp = fopen(path, "r");
    if (!fp) return NULL;
    char buffer[4];
    if (fread(buffer, 1, 4, fp) != 4) goto failure;
    if (memcmp(buffer, "ANC2", 4) != 0) goto failure;
    unsigned char typeChar = 0;
    if (fread(&typeChar, 1, 1, fp) != 1) goto failure;
    if (typeChar < 1 || typeChar > 23) goto failure; // not a known type
    *t = typeChar;
    if (fread(moveMax, 1, 1, fp) != 1) goto failure;

    // calculate the data size and entry count
    long long dataSize = index_type_data_size(*t) + 1;
    fseek(fp, 0, SEEK_END);
    long long offset = ftello(fp);
    fseek(fp, 6, SEEK_SET);
    long long entryCount = (offset - 6) / dataSize;
    // make sure that the file isn't miss-aligned
    if (entryCount * dataSize + 6 != offset) goto failure;
    
    ShardNode * node = (ShardNode *)malloc(sizeof(ShardNode));
    bzero(node, sizeof(ShardNode));
    node->depthRemaining = 0;
    node->nodeData = (unsigned char *)malloc(offset - 6);
    node->nodeDataAlloc = entryCount;
    node->nodeDataSize = entryCount;
    
    // read the file in chunks
    long long dataRead = 0;
    while (dataRead < offset - 6) {
        long long remaining = offset - 6 - dataRead;
        long long chunk = remaining < kBufferSize ? remaining : kBufferSize;
        long long got = fread(&node->nodeData[dataRead], 1, chunk, fp);
        if (got != chunk) {
            shard_node_free(node);
            goto failure;
        }
        dataRead += got;
    }
    
    return node;
    
failure:
    fclose(fp);
    return NULL;
}

static void _index_recursive_write(FILE * output, ShardNode * node,
                                   unsigned char * data, int depth,
                                   int entrySize, int writeSize) {
    if (depth > 0) {
        data[depth - 1] = node->nodeCharacter;
    }
    long long i;
    if (node->depthRemaining == 0) {
        // just write the data, straightup
        for (i = 0; i < node->nodeDataSize; i++) {
            unsigned char * restData = &node->nodeData[(entrySize - depth) * i];
            memcpy(&data[depth], restData, writeSize - depth);
            fwrite(data, 1, writeSize, output);
        }
        return;
    }
    for (i = 0; i < node->subnodeCount; i++) {
        _index_recursive_write(output, node->subnodes[i], data,
                               depth + 1, entrySize, writeSize);
    }
}

static void _index_copy_edge_data(RubiksMap * map, IndexType it, unsigned char * data) {
    int i;
    int max = 5, min = 0;
    int count = 0;
    if (it == IndexTypeEdgeSecond) {
        min = 6;
        max = 11;
    }
    for (i = 0; i < 12 && count < 6; i++) {
        unsigned char piece = map->pieces[i + 8];
        unsigned char pieceNumber = piece & 0xf;
        if (pieceNumber >= min && pieceNumber <= max) {
            count ++;
            data[pieceNumber - min] = i;
            data[pieceNumber - min] |= piece & 0xf0;
        }
    }
}

static void _index_copy_block_data(RubiksMap * map, IndexType it, unsigned char * data) {
    bzero(data, 4);
    int blockIndex = it - 6;
    int i;
    // find the block index corner
    for (i = 0; i < 8; i++) {
        if ((map->pieces[i] & 0xf) == blockIndex) {
            data[0] = i;
            data[0] |= map->pieces[i] & 0xf0;
            break;
        }
    }
    // find the block edges
    for (i = 0; i < 12; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            int seeking = BlockEdgeIndices[blockIndex][j];
            if ((map->pieces[i + 8] & 0xf) == seeking) {
                data[j + 1] = i;
                data[j + 1] |= map->pieces[i + 8] & 0xf0;
            }
        }
    }
}

static void _index_copy_cross_data(RubiksMap * map, IndexType it, unsigned char * data) {
    int crossIndex = it - 14, i;
    const unsigned char * crossIndices = CrossEdgeIndices[crossIndex];
    for (i = 0; i < 12; i++) {
        int j;
        for (j = 0; j < 4; j++) {
            int seeking = crossIndices[j];
            if ((map->pieces[i + 8] & 0xf) == seeking) {
                data[j] = i;
                data[j] |= map->pieces[i + 8] & 0xf0;
            }
        }
    }
}

static void _index_copy_r2f2u_data(RubiksMap * map, IndexType it, unsigned char * data) {
    uint16_t edgesInfo = rubiks_map_edge_orientations(map);
    uint16_t cornersInfo = rubiks_map_corner_orientations(map);
    uint16_t edgeTypes = rubiks_map_topbottom_edge_map(map);

    uint16_t origData[3];
    origData[0] = edgesInfo;
    origData[1] = cornersInfo;
    origData[2] = edgeTypes;
    uint16_t symmetrized[3];
    memcpy(symmetrized, origData, 6);
    map_symmetry_compute_lowest(MapRotationalYSymmetries, 8, origData, symmetrized);
    
    // printf("final: %d, %d, %d\n", symmetrized[0], symmetrized[1], symmetrized[2]);
    
    data[0] = symmetrized[0] & 0xff;
    data[1] = (symmetrized[0] >> 8) & 0xff;
    
    data[2] = symmetrized[1] & 0xff;
    data[3] = (symmetrized[1] >> 8) & 0xff;
    
    data[4] = symmetrized[2] & 0xff;
    data[5] = (symmetrized[2] >> 8) & 0xff;
}

static void _index_copy_edge_permutation_data(RubiksMap * map, unsigned char * data) {
    bzero(data, 6);
    int i;
    for (i = 0; i < 12; i++) {
        uint8_t byte = map->pieces[i + 8];
        int shift = (i % 2 == 0 ? 0 : 4);
        int index = i / 2;
        data[index] |= (byte & 0xf) << shift;
    }
}
