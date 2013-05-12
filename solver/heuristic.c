#include "heuristic.h"

static ShardNode * EdgesFrontIndex;
static ShardNode * EdgesBackIndex;
static ShardNode * CornersIndex;

static _shard_node_move_count(ShardNode * node, 
                              const unsigned char * indices,
                              int indexCount,
                              int defaultResponse);

HeuristicTable * heuristic_table_load(const char * path) {
    HeuristicTable * table = (HeuristicTable *)malloc(sizeof(HeuristicTable));
    
    unsigned char moveMax = 0;
    table->shardNode = index_file_read(path, &table->indexType, &moveMax);
    table->moveMax = moveMax;
    if (!table->shardNode) {
        free(table);
        return NULL;
    }
    
    return table;
}

int heuristic_table_lookup(HeuristicTable * table, const char * data, int len) {
    return _shard_node_move_count(table->shardNode, data, len,
                                  table->moveMax + 1);
}

int heuristic_table_lookup_map(HeuristicTable * table, RubiksMap * map) {
    char data[64];
    int length = index_type_data_size(table->indexType);
    index_type_copy_data(table->indexType, data, map);
    return heuristic_table_lookup(table, data, length);
}

void heuristic_table_free(HeuristicTable * table) {
    shard_node_free(table->shardNode);
    free(table);
}

HeuristicList * heuristic_list_new() {
    HeuristicList * list = (HeuristicList *)malloc(sizeof(HeuristicList));
    bzero(list, sizeof(HeuristicList));
    return list;
}

void heuristic_list_add(HeuristicList * list, HeuristicTable * heuristic) {
    if (!list->tables) {
        list->tables = (HeuristicTable **)malloc(sizeof(void *));
        list->tableCount = 1;
        list->tables[0] = heuristic;
    } else {
        int theSize = sizeof(void *) * (list->tableCount + 1);
        list->tables = (HeuristicTable **)realloc(list->tables, theSize);
        list->tables[list->tableCount] = heuristic;
        list->tableCount++;
    }
}

int heuristic_list_lookup(HeuristicList * list, RubiksMap * map) {
    int i, value = 0;
    for (i = 0; i < list->tableCount; i++) {
        HeuristicTable * table = list->tables[i];
        int moves = heuristic_table_lookup_map(table, map);
        if (moves > value) value = moves;
    }
    return value;
}

int heuristic_list_exceeds(HeuristicList * list, RubiksMap * map, int value) {
    int i;
    for (i = 0; i < list->tableCount; i++) {
        HeuristicTable * table = list->tables[i];
        if (table->moveMax < value) continue;
        if (heuristic_table_lookup_map(table, map) > value) return 1;
    }
    return 0;
}

void heuristic_list_free(HeuristicList * list) {
    int i;
    for (i = 0; i < list->tableCount; i++) {
        heuristic_table_free(list->tables[i]);
    }
    if (list->tables) free(list->tables);
    free(list);
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
