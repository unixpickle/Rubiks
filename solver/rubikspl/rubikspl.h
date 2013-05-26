#include <stdio.h>
#include "representation/cube.h"
#include "solver/heuristic.h"
#include "util/pltools.h"

typedef enum {
    RubiksPlMovesetDefault,
    RubiksPlMovesetDouble,
    RubiksPlMovesetCustom
} RubiksPlMoveset;

typedef struct {
    PlMoveset moveset;
    HeuristicList * heuristics;
} RubiksPl;

void rubikspl_context_free(RubiksPl pl);

void * rubikspl_create_group_identity(void * data);
void rubikspl_free_group_object(void * data, void * object);
void rubikspl_operate(void * data, void * destination, void * object, int operation);
int rubikspl_is_goal(void * data, void * object);
int rubikspl_accepts_sequence(void * data, unsigned char * moves, int count);
int rubikspl_heuristic_exceeds(void * data, void * object, int maxMoves);
void rubikspl_report_solution(void * data, unsigned char * moves, int count);
void rubikspl_report_progress(void * data, time_t duration, long long expandedCount, int depth);
