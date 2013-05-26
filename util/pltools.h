#ifndef __PLTOOLS_H__
#define __PLTOOLS_H__

#include "representation/cube.h"
#include <stdio.h>

typedef enum {
    PlMovesetDefault,
    PlMovesetDouble,
    PlMovesetCustom
} PlMovesetType;

typedef struct {
    RubiksMap ** operations;
    char ** descriptions;
    int operationCount;
    int maxIndepFaces;
    PlMovesetType moveset;
} PlMoveset;

int standard_operations_validate(const char * moves, int count);

void pl_moveset_initialize(PlMoveset * moveset);
void pl_moveset_initialize_double(PlMoveset * moveset);
void pl_moveset_initialize_custom(PlMoveset * moveset, RubiksMap ** maps,
                                  char ** descriptions, int count);
void pl_moveset_free(PlMoveset * moveset);
int pl_moveset_accepts_sequence(PlMoveset * moveset, unsigned char * moves, int count);
void pl_moveset_print_solution(PlMoveset * moveset, unsigned char * moves, int count);

#endif