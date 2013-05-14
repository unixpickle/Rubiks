#include <stdio.h>
#include "representation/cube.h"
#include "indexing/subproblemindices.h"
#include "solver/heuristic.h"
#include "solver/pltools.h"

typedef struct {
    HeuristicTable * heuristic;
    RubiksMap ** operations;
} R2F2UPl;

void r2f2upl_initialize(R2F2UPl * pl);
void r2f2upl_free(R2F2UPl pl);

void * r2f2upl_create_group_identity(void * data);
void r2f2upl_free_group_object(void * data, void * object);
void r2f2upl_operate(void * data, void * destination, void * object, int operation);
int r2f2upl_is_goal(void * data, void * object);
int r2f2upl_accepts_sequence(void * data, unsigned char * moves, int count);
int r2f2upl_heuristic_exceeds(void * data, void * object, int maxMoves);
void r2f2upl_report_solution(void * data, unsigned char * moves, int count);
void r2f2upl_report_progress(void * data, time_t duration, long long expandedCount, int depth);
