#include "cubeindex.h"
#include "subproblemindices.h"

int heuristic_load_index_files(const char * corners, const char * edgeFront, const char * edgeBack);
int heuristic_minimum_moves(RubiksMap * map);
int heuristic_exceeds_moves(RubiksMap * map, int maximum);
void heuristic_free_all();
