/**
 * This API was designed to count the minimum number of moves
 * required to achieve various configurations of a subset
 * of the full 3x3x3 rubik's cube data.
 */

#include "shard.h"
#include "cube.h"
#include "index.h"

typedef struct {
    IndexType indexType;
    int shardDepth;
    int maximumDepth;
    RubiksMap * identity;
} CCUserInfo;

// retreiving and working with tables
/**
 * Computes a full table of all the possible configurations of the map
 * along the given significant indices.
 */
ShardNode * cc_compute_table(CCUserInfo info);
