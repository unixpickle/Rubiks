#include "crosspl.h"

void crosspl_initialize(CrossPl * pl) {
    pl->operations = cube_standard_face_turns();
}

void crosspl_free(CrossPl pl) {
    int i;
    for (i = 0; i < 18; i++) {
        rubiks_map_free(pl.operations[i]);
    }
    free(pl.operations);
}

void * crosspl_create_group_identity(void * data) {
    return rubiks_map_new_identity();
}

void crosspl_free_group_object(void * data, void * object) {
    rubiks_map_free((RubiksMap *)object);
}

void crosspl_operate(void * data, void * destination, void * object, int operation) {
    CrossPl * pl = (CrossPl *)data;
    rubiks_map_multiply(destination, pl->operations[operation], object);
}

int crosspl_is_goal(void * data, void * object) {
    CrossPl * pl = (CrossPl *)data;
    RubiksMap * map = (RubiksMap *)object;
    const unsigned char * edgeIndices = CrossEdgeIndices[pl->crossIndex];
    int i;
    for (i = 0; i < 4; i++) {
        if (map->pieces[8 + edgeIndices[i]] != edgeIndices[i]) {
            return 0;
        }
    }
    return 1;
}

int crosspl_accepts_sequence(void * data, unsigned char * moves, int count) {
    return standard_operations_validate(moves, count);
}

int crosspl_heuristic_exceeds(void * data, void * object, int maxMoves) {
    CrossPl * pl = (CrossPl *)data;
    if (!pl->heuristic) {
        return 0;
    }
    int count = heuristic_table_lookup_map(pl->heuristic, (RubiksMap *)object);
    return (count > maxMoves);
}

void crosspl_report_solution(void * data, unsigned char * moves, int count) {
    printf("Found solution: ");
    cube_print_standard_solution(moves, count);
    printf("\n");
}

void crosspl_report_progress(void * data, time_t duration, long long expandedCount, int depth) {
    printf("Expanded %lld nodes [depth = %d, time = %lld seconds]\n", 
           expandedCount, depth, (long long)duration);
}
