#include "r2f2upl.h"

void r2f2upl_initialize(R2F2UPl * pl) {
    pl_moveset_initialize(&pl->moveset);
}

void r2f2upl_free(R2F2UPl pl) {
    pl_moveset_free(&pl.moveset);
    if (pl.heuristic) {
        heuristic_table_free(pl.heuristic);
    }
}

void * r2f2upl_create_group_identity(void * data) {
    return rubiks_map_new_identity();
}

void r2f2upl_free_group_object(void * data, void * object) {
    rubiks_map_free((RubiksMap *)object);
}

void r2f2upl_operate(void * data, void * destination, void * object, int operation) {
    R2F2UPl * pl = (R2F2UPl *)data;
    rubiks_map_multiply(destination, pl->moveset.operations[operation], object);
}

int r2f2upl_is_goal(void * data, void * object) {
    R2F2UPl * pl = (R2F2UPl *)data;
    RubiksMap * map = (RubiksMap *)object;
    if (rubiks_map_corner_orientations(map) != 0x5555) return 0;
    uint16_t edgeInfo = rubiks_map_topbottom_edge_map(map);
    if (edgeInfo != 0xd75) return 0;
    if (rubiks_map_edge_orientations(map) != 0xfff) return 0;
    return 1;
}

int r2f2upl_accepts_sequence(void * data, unsigned char * moves, int count) {
    R2F2UPl * pl = (R2F2UPl *)data;
    return pl_moveset_accepts_sequence(&pl->moveset, moves, count);
}

int r2f2upl_heuristic_exceeds(void * data, void * object, int maxMoves) {
    R2F2UPl * pl = (R2F2UPl *)data;
    if (!pl->heuristic) {
        return 0;
    }
    int count = heuristic_table_lookup_map(pl->heuristic, (RubiksMap *)object);
    return (count > maxMoves);
}

void r2f2upl_report_solution(void * data, unsigned char * moves, int count) {
    R2F2UPl * pl = (R2F2UPl *)data;
    printf("Found solution: ");
    pl_moveset_print_solution(&pl->moveset, moves, count);
    printf("\n");
}

void r2f2upl_report_progress(void * data, time_t duration, long long expandedCount, int depth) {
    printf("Expanded %lld nodes [depth = %d, time = %lld seconds]\n", 
           expandedCount, depth, (long long)duration);
}
