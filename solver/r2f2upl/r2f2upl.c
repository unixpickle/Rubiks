#include "r2f2upl.h"

void r2f2upl_initialize(R2F2UPl * pl) {
    pl->operations = cube_standard_face_turns();
}

void r2f2upl_free(R2F2UPl pl) {
    int i;
    for (i = 0; i < 18; i++) {
        rubiks_map_free(pl.operations[i]);
    }
    free(pl.operations);
}

void * r2f2upl_create_group_identity(void * data) {
    return rubiks_map_new_identity();
}

void r2f2upl_free_group_object(void * data, void * object) {
    rubiks_map_free((RubiksMap *)object);
}

void r2f2upl_operate(void * data, void * destination, void * object, int operation) {
    R2F2UPl * pl = (R2F2UPl *)data;
    rubiks_map_multiply(destination, pl->operations[operation], object);
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
    return standard_operations_validate(moves, count);
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
    printf("Found solution: ");
    cube_print_standard_solution(moves, count);
    printf("\n");
}

void r2f2upl_report_progress(void * data, time_t duration, long long expandedCount, int depth) {
    printf("Expanded %lld nodes [depth = %d, time = %lld seconds]\n", 
           expandedCount, depth, (long long)duration);
}
