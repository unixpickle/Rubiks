#include <stdio.h>
#include "representation/cube.h"
#include "indexing/subproblemindices.h"
#include "solver/heuristic.h"
#include "util/pltools.h"

typedef struct {
    HeuristicTable * heuristic;
    RubiksMap ** operations;
    int blockIndex;
} BlockPl;

void blockpl_initialize(BlockPl * pl);
void blockpl_free(BlockPl pl);

void * blockpl_create_group_identity(void * data);
void blockpl_free_group_object(void * data, void * object);
void blockpl_operate(void * data, void * destination, void * object, int operation);
int blockpl_is_goal(void * data, void * object);
int blockpl_accepts_sequence(void * data, unsigned char * moves, int count);
int blockpl_heuristic_exceeds(void * data, void * object, int maxMoves);
void blockpl_report_solution(void * data, unsigned char * moves, int count);
void blockpl_report_progress(void * data, time_t duration, long long expandedCount, int depth);
