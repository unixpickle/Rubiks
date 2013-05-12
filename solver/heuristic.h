#ifndef __HEURISTIC_H__
#define __HEURISTIC_H__

#include "indexing/index.h"
#include "representation/rubiksmap.h"

typedef struct {
    int moveMax;
    IndexType indexType;
    ShardNode * shardNode;
} HeuristicTable;

typedef struct {
    HeuristicTable ** tables;
    int tableCount;
} HeuristicList;

HeuristicTable * heuristic_table_load(const char * path);
int heuristic_table_lookup(HeuristicTable * table, const char * data, int len);
int heuristic_table_lookup_map(HeuristicTable * table, RubiksMap * map);
void heuristic_table_free(HeuristicTable * table);

HeuristicList * heuristic_list_new();
void heuristic_list_add(HeuristicList * list, HeuristicTable * heuristic);
int heuristic_list_lookup(HeuristicList * list, RubiksMap * map);
int heuristic_list_exceeds(HeuristicList * list, RubiksMap * map, int value);
void heuristic_list_free(HeuristicList * list);

#endif