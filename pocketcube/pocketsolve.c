#include <stdio.h>
#include <stdlib.h>

#include "pocketmap.h"
#include "representation/cube.h"
#include "indexing/index.h"

static ShardNode * cornersNode = NULL;
static unsigned char maxMoves = 0;
static PocketMap * operations[18];

int search_main(PocketMap * node, int depth, int maxDepth, unsigned char * moves);
int moves_exceeds(PocketMap * map, int remaining);
int heuristic_lookup(PocketMap * map);

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <corners.anc2>\n", argv[0]);
        return 1;
    }
    const char * cornersPath = argv[1];
    
    IndexType type;
    cornersNode = index_file_read(cornersPath, &type, &maxMoves);
    if (type != IndexTypeCorners || !cornersNode) {
        fprintf(stderr, "error: Invalid index file type.\n");
        if (cornersNode) shard_node_free(cornersNode);
        return 1;
    }
    
    int i, j;
    
    PocketMap * map = pocket_map_user_input();
    if (!map) {
        fprintf(stderr, "error: invalid user input\n");
        shard_node_free(cornersNode);
        return 1;
    }
    
    printf("\ncube data (hex): ");
    for (i = 0; i < 8; i++) {
        printf(" %02x", map->pieces[i]);
    }
    printf("\n\n");
    
    PocketMap * identity = pocket_map_new_identity();
    
    for (i = 0; i < 6; i++) {
        PocketMap * map = pocket_map_new_identity();
        PocketMap * doubleTurn = pocket_map_new_identity();
        pocket_map_operate(map, identity, i);
        pocket_map_operate(doubleTurn, map, i);
        operations[i] = map;
        operations[i + 6] = pocket_map_inverse(map);
        operations[i + 12] = doubleTurn;
    }
    
    char moves[12];
    for (i = 0; i <= 11; i++) {
        printf("Trying depth %d\n", i);
        if (search_main(map, 0, i, moves)) {
            printf("Found solution: ");
            cube_print_standard_solution(moves, i);
            printf("\n");
        }
    }
    
    shard_node_free(cornersNode);
    
    return 0;
}

int search_main(PocketMap * node, int depth, int maxDepth, unsigned char * moves) { 
    if (depth == maxDepth) {
        if (pocket_map_is_identity(node)) {
            return 1;
        }
        return 0;
    }
    if (moves_exceeds(node, maxDepth - depth)) return 0;
    PocketMap * nextMap = pocket_map_new_identity();
    int i;
    for (i = 0; i < 18; i++) {
        moves[depth] = i;
        pocket_map_multiply(nextMap, operations[i], node);
        if (search_main(nextMap, depth + 1, maxDepth, moves)) {
            pocket_map_free(nextMap);
            return 1;
        }
    }
    pocket_map_free(nextMap);
    
    return 0;
}

int moves_exceeds(PocketMap * map, int remaining) {
    if (heuristic_lookup(map) > remaining) return 1;
    return 0;
}

int heuristic_lookup(PocketMap * map) {
    unsigned char * result = shard_node_base_lookup(cornersNode, map->pieces, 9, 1);
    if (result) {
        return result[8];
    }
    return maxMoves + 1;
}
