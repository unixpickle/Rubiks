#include "solveagent.h"
#include "heuristic.h"
#include "representation/cube.h"
#include <string.h>

int parse_solve_agent_argument(SAUserInfo * info, const char * argument);

// return 0 if no argument, 1 if success, -1 if failed to load
int parse_heuristic_argument(HeuristicTable ** out, const char * argument);