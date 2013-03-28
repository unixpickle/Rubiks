#include "heuristic.h"

static ShardNode * EdgesFrontIndex;
static ShardNode * EdgesBackIndex;
static ShardNode * CornersIndex;

static _shard_node_move_count(ShardNode * node, 
                              const unsigned char * indices,
                              int indexCount,
                              int defaultResponse);

int heuristic_load_index_files(const char * corners, const char * edgeFront, const char * edgeBack) {
    IndexType t;
    unsigned char moveMax;
    EdgesFrontIndex = index_file_read(edgeFront, &t, &moveMax);
    if (t != IndexTypeEdgeFront || moveMax < 9 || !EdgesFrontIndex) {
        if (EdgesFrontIndex) shard_node_free(EdgesFrontIndex);
        fprintf(stderr, "error: invalid front edge index.\n");
        return 0;
    }
    EdgesBackIndex = index_file_read(edgeBack, &t, &moveMax);
    if (t != IndexTypeEdgeBack || moveMax < 9 || !EdgesBackIndex) {
        shard_node_free(EdgesFrontIndex);
        if (EdgesBackIndex) shard_node_free(EdgesBackIndex);
        fprintf(stderr, "error: invalid back edge index.\n");
        return 0;
    }
    CornersIndex = index_file_read(corners, &t, &moveMax);
    if (t != IndexTypeCorners || moveMax < 10 || !CornersIndex) {
        shard_node_free(EdgesFrontIndex);
        shard_node_free(EdgesBackIndex);
        if (CornersIndex) shard_node_free(CornersIndex);
        fprintf(stderr, "error: invalid corners index.\n");
        return 0;
    }
    return 1;
}

int heuristic_minimum_moves(RubiksMap * map) {
    int cornerCount = _shard_node_move_count(CornersIndex, 
                                             map->pieces,
                                             8, 11);
    if (cornerCount >= 10) return cornerCount;
    int edgeFront = _shard_node_move_count(EdgesFrontIndex, 
                                           &map->pieces[8],
                                           6, 10);
    if (edgeFront >= 10) return edgeFront;
    int edgeBack = _shard_node_move_count(EdgesBackIndex, 
                                          &map->pieces[14],
                                          6, 10);
    if (edgeBack >= 10) return edgeBack;
    if (edgeBack > cornerCount) {
        if (edgeBack > edgeFront) return edgeBack;
        else return edgeFront;
    } else {
        if (cornerCount > edgeFront) return cornerCount;
        else return edgeFront;
    }
}

int heuristic_exceeds_moves(RubiksMap * map, int maximum) {
    if (maximum > 11) return 0;
    int cornerCount = _shard_node_move_count(CornersIndex, 
                                             map->pieces,
                                             8, 11);
    if (cornerCount > maximum) return 1;
    int edgeFront = _shard_node_move_count(EdgesFrontIndex, 
                                           &map->pieces[8],
                                           6, 10);
    if (edgeFront > maximum) return 1;
    int edgeBack = _shard_node_move_count(EdgesBackIndex, 
                                          &map->pieces[14],
                                          6, 10);
    if (edgeBack > maximum) return 1;
    return 0;
}

void heuristic_free_all() {
    shard_node_free(EdgesFrontIndex);
    shard_node_free(EdgesBackIndex);
    shard_node_free(CornersIndex);
}

static _shard_node_move_count(ShardNode * node, 
                              const unsigned char * indices,
                              int indexCount,
                              int defaultResponse) {
    unsigned char * result = shard_node_base_lookup(node, indices,
                                                    indexCount + 1,
                                                    1);
    if (result) {
        return result[indexCount];
    }
    return defaultResponse;
}
