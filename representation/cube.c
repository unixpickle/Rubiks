#include "cube.h"

static RubiksMap * _map_with_index_identifier(int index);

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
    int i;
    for (i = 0; i < length; i++) {
        printf("%s%s", (i == 0 ? "" : " "), names[moves[i]]);
    }
}

RubiksMap * cube_for_algorithm(const char * algorithm) {
    const char * names[] = {
        "U", "D'", "R'", "L", "F", "B'",
        "U'", "D", "R", "L'", "F'", "B",
        "U2", "D2", "R2", "L2", "F2", "B2"
    };
    char move[3];
    const char * faceLetters = "UDRLFB";
    int i, j;
    
    RubiksMap * temp = rubiks_map_new_identity();
    RubiksMap * cube = rubiks_map_new_identity();
    
    for (i = 0; i < strlen(algorithm); i++) {
        int isAlgoCharacter = 0;
        for (j = 0; j < 6; j++) {
            if (algorithm[i] == faceLetters[j]) {
                isAlgoCharacter = 1;
            }
        }
        if (!isAlgoCharacter) continue;
        int isTwoCharacters = 0;
        if (i + 1 < strlen(algorithm)) {
            if (algorithm[i + 1] == '\'' || algorithm[i + 1] == '2') {
                isTwoCharacters = 1;
            }
        }
        bzero(move, 3);
        memcpy(move, &algorithm[i], isTwoCharacters ? 2 : 1);
        
        int index = 0;
        for (j = 0; j < 18; j++) {
            if (strcmp(names[j], move) == 0) {
                index = j;
                break;
            }
        }
        
        RubiksMap * operation = _map_with_index_identifier(index);
        rubiks_map_multiply(temp, operation, cube);
        rubiks_map_copy_into(cube, temp);
        rubiks_map_free(operation);
        
        if (isTwoCharacters) i++;
    }
    rubiks_map_free(temp);
    return cube;
}

static RubiksMap * _map_with_index_identifier(int index) {
    if (index < 6) {
        RubiksMap * identity = rubiks_map_new_identity();
        RubiksMap * result = rubiks_map_new_identity();
        rubiks_map_operate(result, identity, index);
        rubiks_map_free(identity);
        return result;
    } else if (index < 12) {
        RubiksMap * identity = rubiks_map_new_identity();
        RubiksMap * regular = rubiks_map_new_identity();
        rubiks_map_operate(regular, identity, index - 6);
        RubiksMap * inverse = rubiks_map_inverse(regular);
        rubiks_map_free(regular);
        rubiks_map_free(identity);
        return inverse;
    } else {
        RubiksMap * identity = rubiks_map_new_identity();
        RubiksMap * map1 = rubiks_map_new_identity();
        RubiksMap * map2 = rubiks_map_new_identity();
        rubiks_map_operate(map1, identity, index - 12);
        rubiks_map_operate(map2, map1, index - 12);
        rubiks_map_free(map1);
        rubiks_map_free(identity);
        return map2;
    }
}
