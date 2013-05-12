#include <stdio.h>
#include "representation/cube.h"
#include "heuristic.h"

typedef struct {
    HeuristicTable * edgeHeuristic;
    RubiksMap ** operations;
} OrientPl;

void orientpl_initialize(OrientPl * pl);
void orientpl_free(OrientPl pl);

void * orientpl_create_group_identity(void * data);
void orientpl_free_group_object(void * data, void * object);
void orientpl_operate(void * data, void * destination, void * object, int operation);
int orientpl_is_goal(void * data, void * object);
int orientpl_accepts_sequence(void * data, unsigned char * moves, int count);
int orientpl_heuristic_exceeds(void * data, void * object, int maxMoves);
void orientpl_report_solution(void * data, unsigned char * moves, int count);
void orientpl_report_progress(void * data, time_t duration, long long expandedCount, int depth);
