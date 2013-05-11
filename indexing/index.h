#include "rubiksmap.h"
#include "shard.h"

typedef enum {
    IndexTypeUnknown = 0,
    IndexTypeEdgeFront = 1,
    IndexTypeEdgeBack = 2,
    IndexTypeEdgeAll = 3,
    IndexTypeCorners = 4,
    IndexTypeEO = 5
} IndexType;

IndexType index_type_from_string(const char * str);
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
