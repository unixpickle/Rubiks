#include "heuristic.h"

static cube_index_t * EdgesFrontIndex;
static cube_index_t * EdgesBackIndex;
static cube_index_t * CornersIndex;

int heuristic_load_index_files(const char * corners, const char * edgeFront, const char * edgeBack) {
    EdgesFrontIndex = cube_index_load(edgeFront);
    EdgesBackIndex = cube_index_load(edgeBack);
    CornersIndex = cube_index_load(corners);
    if (EdgesFrontIndex && EdgesBackIndex && CornersIndex) return 1;
    return 0;
}

int heuristic_minimum_moves(RubiksMap * map) {
    int cornerCount = cube_index_lookup_moves(CornersIndex, map, CubeCornerIndices);
    if (cornerCount >= 10) return 11;
    int edgeFront = cube_index_lookup_moves(EdgesFrontIndex, map, CubeFrontIndices);
    if (edgeFront == 10) return 10;
    int edgeBack = cube_index_lookup_moves(EdgesBackIndex, map, CubeBackIndices);
    if (edgeBack > cornerCount) {
        if (edgeBack > edgeFront) return edgeBack;
        else return edgeFront;
    } else {
        if (cornerCount > edgeFront) return cornerCount;
        else return edgeFront;
    }
}

void heuristic_free_all() {
    cube_index_free(EdgesFrontIndex);
    cube_index_free(EdgesBackIndex);
    cube_index_free(CornersIndex);
}
