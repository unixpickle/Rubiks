#include "cube.h"

RubiksMap ** cube_standard_face_turns() {
    RubiksMap ** operations = malloc(sizeof(RubiksMap *) * 18);
    int i;
    RubiksMap * identity = rubiks_map_new_identity();
    for (i = 0; i < 6; i++) {
        operations[i] = rubiks_map_new_identity();
        rubiks_map_operate(operations[i], identity, i);
    }
    rubiks_map_free(identity);
    for (i = 0; i < 6; i++) {
        RubiksMap * doubleTurn = rubiks_map_new_identity();
        rubiks_map_multiply(doubleTurn, operations[i], operations[i]);
        operations[i + 6] = rubiks_map_inverse(operations[i]);
        operations[i + 12] = doubleTurn;
    }
    return operations;
}

void cube_print_solution_key() {
    printf(" 0 - top     6 - top inverse     12 - top double\n");
    printf(" 1 - bottom  7 - bottom inverse  13 - bottom double\n");
    printf(" 2 - right   8 - right inverse   14 - right double\n");
    printf(" 3 - left    9 - left inverse    15 - left double\n");
    printf(" 4 - front   10 - front inverse  16 - front double\n");
    printf(" 5 - back    11 - back inverse   17 - back double\n");
}
