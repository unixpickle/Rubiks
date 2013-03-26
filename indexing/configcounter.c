#include "configcounter.h"

#define kConfigCounterBufferSize 16 // number of entries per allocation
#define kConfigCounterMoveCount 18

static int cubesAdded = 0;

static void _cc_recursive_search(CCUserInfo * userInfo,
                                 int depth,
                                 int maxDepth,
                                 char * previousMoves,
                                 RubiksMap * nodeMap,
                                 RubiksMap ** operations,
                                 ShardNode * root,
                                 int indexCount);

ShardNode * cc_compute_table(CCUserInfo info) {
    info.nodesExpanded = 0;
    ShardNode * baseNode = shard_node_new(info.shardDepth);
    RubiksMap ** operations = cube_standard_face_turns();
    char previousMoves[1];
    int i;
    for (i = 0; i <= info.maximumDepth; i++) {
        printf("Performing search with depth of %d\n", i);
        _cc_recursive_search(&info, 0, i, previousMoves,
                             info.identity, operations, baseNode,
                             index_type_data_size(info.indexType));
    }
    for (i = 0; i < kConfigCounterMoveCount; i++) {
        rubiks_map_free(operations[i]);
    }
    free(operations);
    printf("Expanded a total of %d nodes.\n", info.nodesExpanded);
    return baseNode;
}

static void _cc_recursive_search(CCUserInfo * userInfo,
                                 int depth,
                                 int maxDepth,
                                 char * previousMoves,
                                 RubiksMap * nodeMap,
                                 RubiksMap ** operations,
                                 ShardNode * root,
                                 int indexCount) {
    // generate the map data
    char mapData[22];
    index_type_copy_data(userInfo->indexType, mapData, nodeMap);
    mapData[indexCount] = depth;
    mapData[indexCount + 1] = maxDepth;
    // find if the rubiks information already exists in the table
    ShardNode * baseNode = shard_node_search_base(root, mapData, indexCount, 1);
    
    int entryDataLength = indexCount + 2 - userInfo->shardDepth;
    unsigned char * entry = shard_node_base_lookup(baseNode, 
                                                   &mapData[userInfo->shardDepth],
                                                   entryDataLength, 2);
    // if the information exists, check if we are done with this node
    if (entry) {
        if (userInfo->indexType == IndexTypeCorners ||
            userInfo->indexType == IndexTypeEdgeAll) {
            if (entry[entryDataLength - 2] < depth) return;
            if (memcmp(&entry[entryDataLength - 2], &mapData[indexCount], 2) == 0) return;
        } else {
            if (entry[entryDataLength - 1] == maxDepth && depth == maxDepth) return;
        }
        // prevent future nodes from being explored
        entry[entryDataLength - 1] = maxDepth;
    } else {
        int res = shard_node_base_add(baseNode, &mapData[userInfo->shardDepth],
                                      entryDataLength, 2);
        userInfo->nodesExpanded++;
        if (userInfo->nodesExpanded % 65536 == 0) {
            printf("Found %d cubes\n", userInfo->nodesExpanded);
        }
    }
    if (depth == maxDepth) return;
    
    // use recursion to explore child nodes
    char * nextPrevious = (char *)malloc(depth + 1);
    if (depth > 0) memcpy(nextPrevious, previousMoves, depth);
    int i;
    RubiksMap * deeperCube = rubiks_map_new_identity();
    char lastMove = depth > 0 ? previousMoves[depth - 1] : -1;
    for (i = 0; i < kConfigCounterMoveCount; i++) {
        // remove redundant commutative chains
        if (depth > 0) {
            if (lastMove % 2 == 1) {
                // on an odd move; this is where we limit it
                if ((i % 6) + 1 == lastMove % 6) {
                    // we don't perform a commuting even move from an odd move
                    continue;
                }
            }
            if (lastMove >= 12 && i == lastMove) continue;
            if (lastMove < 12 && i % 6 == lastMove % 6 && i != lastMove) {
                // we are making an inverse of the previous move
                continue;
            }
        }
        nextPrevious[depth] = i;
        rubiks_map_multiply(deeperCube, operations[i], nodeMap);
        _cc_recursive_search(userInfo,
                             depth + 1,
                             maxDepth,
                             nextPrevious,
                             deeperCube,
                             operations,
                             root,
                             indexCount);
    }
    free(nextPrevious);
    rubiks_map_free(deeperCube);
}
