#include "rubikspl.h"

void rubikspl_context_free(RubiksPl pl) {
    pl_moveset_free(&pl.moveset);
    heuristic_list_free(pl.heuristics);
}

// called by solver agent

void * rubikspl_create_group_identity(void * data) {
    return rubiks_map_new_identity();
}

void rubikspl_free_group_object(void * data, void * object) {
    rubiks_map_free((RubiksMap *)object);
}

void rubikspl_operate(void * data, void * destination, void * object, int operation) {
    RubiksPl * pl = (RubiksPl *)data;
    rubiks_map_multiply(destination, pl->moveset.operations[operation], object);
}

int rubikspl_is_goal(void * data, void * object) {    
    return rubiks_map_is_identity(object);
}

int rubikspl_accepts_sequence(void * data, unsigned char * moves, int count) {
    RubiksPl * pl = (RubiksPl *)data;
    return pl_moveset_accepts_sequence(&pl->moveset, moves, count);
}

int rubikspl_heuristic_exceeds(void * data, void * object, int maxMoves) {
    RubiksPl * pl = (RubiksPl *)data;
    return heuristic_list_exceeds(pl->heuristics, (RubiksMap *)object, maxMoves);
}

void rubikspl_report_solution(void * data, unsigned char * moves, int count) {
    RubiksPl * pl = (RubiksPl *)data;
    printf("Found solution:");
    pl_moveset_print_solution(&pl->moveset, moves, count);
    printf("\n");
}

void rubikspl_report_progress(void * data, time_t duration, long long expandedCount, int depth) {
    printf("Expanded %lld nodes [depth = %d, time = %lld seconds]\n", 
           expandedCount, depth, (long long)duration);
}
