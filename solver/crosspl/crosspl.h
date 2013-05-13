#include <stdio.h>
#include "representation/cube.h"
#include "indexing/subproblemindices.h"
#include "solver/heuristic.h"
#include "solver/pltools.h"

typedef struct {
    HeuristicTable * heuristic;
    RubiksMap ** operations;
    int crossIndex;
} CrossPl;

void crosspl_initialize(CrossPl * pl);
void crosspl_free(CrossPl pl);

void * crosspl_create_group_identity(void * data);
void crosspl_free_group_object(void * data, void * object);
void crosspl_operate(void * data, void * destination, void * object, int operation);
int crosspl_is_goal(void * data, void * object);
int crosspl_accepts_sequence(void * data, unsigned char * moves, int count);
int crosspl_heuristic_exceeds(void * data, void * object, int maxMoves);
void crosspl_report_solution(void * data, unsigned char * moves, int count);
void crosspl_report_progress(void * data, time_t duration, long long expandedCount, int depth);
