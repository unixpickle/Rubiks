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

void cube_print_standard_solution(const unsigned char * moves, int length) {
    const char * names[] = {
        "U", "D'", "R'", "L", "F", "B'",
        "U'", "D", "R", "L'", "F'", "B",
        "U2", "D2", "R2", "L2", "F2", "B2"
    };
    printf("Found solution: ");
    int i;
    for (i = 0; i < length; i++) {
        printf("%s%s", (i == 0 ? "" : " "), names[moves[i]]);
    }
    printf("\n");
}

