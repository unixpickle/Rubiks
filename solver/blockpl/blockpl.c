#include "blockpl.h"

void blockpl_initialize(BlockPl * pl) {
    pl->operations = cube_standard_face_turns();
}

void blockpl_free(BlockPl pl) {
    int i;
    for (i = 0; i < 18; i++) {
        rubiks_map_free(pl.operations[i]);
    }
    free(pl.operations);
}

void * blockpl_create_group_identity(void * data) {
    return rubiks_map_new_identity();
}

void blockpl_free_group_object(void * data, void * object) {
    rubiks_map_free((RubiksMap *)object);
}

void blockpl_operate(void * data, void * destination, void * object, int operation) {
    BlockPl * pl = (BlockPl *)data;
    rubiks_map_multiply(destination, pl->operations[operation], object);
}

int blockpl_is_goal(void * data, void * object) {
    BlockPl * pl = (BlockPl *)data;
    RubiksMap * map = (RubiksMap *)object;
    const unsigned char * edgeIndices = BlockEdgeIndices[pl->blockIndex];
    if (map->pieces[pl->blockIndex] != pl->blockIndex) {
        return 0;
    }
    int i;
    for (i = 0; i < 3; i++) {
        if (map->pieces[8 + edgeIndices[i]] != edgeIndices[i]) {
            return 0;
        }
    }
    return 1;
}

int blockpl_accepts_sequence(void * data, unsigned char * moves, int count) {
    return standard_operations_validate(moves, count);
}

int blockpl_heuristic_exceeds(void * data, void * object, int maxMoves) {
    BlockPl * pl = (BlockPl *)data;
    if (!pl->heuristic) {
        return 0;
    }
    int count = heuristic_table_lookup_map(pl->heuristic, (RubiksMap *)object);
    return (count > maxMoves);
}

void blockpl_report_solution(void * data, unsigned char * moves, int count) {
    printf("Found solution: ");
    cube_print_standard_solution(moves, count);
    printf("\n");
}

void blockpl_report_progress(void * data, time_t duration, long long expandedCount, int depth) {
    printf("Expanded %lld nodes [depth = %d, time = %lld seconds]\n", 
           expandedCount, depth, (long long)duration);
}
