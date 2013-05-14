#include "representation/rubiksmap.h"
#include "representation/stickerindices.h"
#include "mapsymmetry.h"
#include "shard.h"
#include "subproblemindices.h"

typedef enum {
    IndexTypeUnknown = 0,
    IndexTypeEdgeFront = 1,
    IndexTypeEdgeBack = 2,
    IndexTypeEdgeAll = 3,
    IndexTypeCorners = 4,
    IndexTypeEO = 5,
    IndexTypeBlock0 = 6,
    IndexTypeBlock1 = 7,
    IndexTypeBlock2 = 8,
    IndexTypeBlock3 = 9,
    IndexTypeBlock4 = 10,
    IndexTypeBlock5 = 11,
    IndexTypeBlock6 = 12,
    IndexTypeBlock7 = 13,
    IndexTypeCross1 = 14,
    IndexTypeCross2 = 15,
    IndexTypeCross3 = 16,
    IndexTypeCross4 = 17,
    IndexTypeCross5 = 18,
    IndexTypeCross6 = 19,
    IndexTypeR2F2UGroup = 20
} IndexType;

IndexType index_type_from_string(const char * str);
const char * index_type_to_string(IndexType type);
int index_type_data_size(IndexType it);
void index_type_copy_data(IndexType it, unsigned char * data, RubiksMap * map);
void index_file_write(IndexType it,
                      int writeSize,
                      int dataSize,
                      int maxDepth,
                      ShardNode * baseNode,
                      FILE * output);
ShardNode * index_file_read(const char * path,
                            IndexType * t,
                            unsigned char * moveMax);
