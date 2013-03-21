#include <stdint.h>
#include "rubiksmap.h"

typedef struct {
    int entrySize;
    int defaultMax;
    int entryCount;
    int indexCount;
    unsigned char * configurationData;
} cube_index_t;

cube_index_t * cube_index_load(const char * path);
int cube_index_lookup_moves(cube_index_t * index, RubiksMap * map, const unsigned char * significantIndices);
void cube_index_free(cube_index_t * index);

