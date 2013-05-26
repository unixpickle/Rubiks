#include "pltools.h"

static int _pl_verify_faceturns(PlMoveset * ms, unsigned char * moves, int numMoves);

int standard_operations_validate(const char * moves, int count) {
    if (count == 1) return 1;
    unsigned char lastMove = moves[count - 2];
    unsigned char currentMove = moves[count - 1];
    if (lastMove % 2 == 1) {
        // on an odd move; this is where we limit it
        if ((currentMove % 6) + 1 == lastMove % 6) {
            // we don't perform a commuting even move from an odd move
            return 0;
        }
    }
    if (currentMove == lastMove) return 0;
    if (currentMove % 6 == lastMove % 6) {
        // we are making another turn on the same face :(
        return 0;
    }
    return 1;
}

void pl_moveset_initialize(PlMoveset * moveset) {
    const char * names[] = {
        "U", "D'", "R'", "L", "F", "B'",
        "U'", "D", "R", "L'", "F'", "B",
        "U2", "D2", "R2", "L2", "F2", "B2"
    };
    
    moveset->operations = cube_standard_face_turns();
    moveset->descriptions = (char **)malloc(sizeof(char *) * 18);
    int i;
    for (i = 0; i < 18; i++) {
        char * str = (char *)malloc(strlen(names[i]) + 1);
        strcpy(str, names[i]);
        moveset->descriptions[i] = str;
    }
    moveset->operationCount = 18;
    moveset->moveset = PlMovesetDefault;
    moveset->maxIndepFaces = 6;
}

void pl_moveset_initialize_double(PlMoveset * moveset) {
    const char * moves[] = {
        "F2", "B2", "U2", "D2", "R2", "L2"
    };
    moveset->operations = (RubiksMap **)malloc(sizeof(void *) * 6);
    moveset->descriptions = (char **)malloc(sizeof(char *) * 6);
    int i;
    for (i = 0; i < 6; i++) {
        moveset->operations[i] = cube_for_algorithm(moves[i]);
        moveset->descriptions[i] = (char *)malloc(strlen(moves[i]) + 1);
        strcpy(moveset->descriptions[i], moves[i]);
    }
    moveset->operationCount = 6;
    moveset->moveset = PlMovesetDouble;
    moveset->maxIndepFaces = 6;
}

void pl_moveset_initialize_custom(PlMoveset * moveset, RubiksMap ** maps,
                                  char ** descriptions, int count) {
    moveset->operations = maps;
    moveset->descriptions = descriptions;
    moveset->operationCount = count;
    moveset->moveset = PlMovesetCustom;                           
}

void pl_moveset_free(PlMoveset * moveset) {
    int i;
    for (i = 0; i < moveset->operationCount; i++) {
        rubiks_map_free(moveset->operations[i]);
        free(moveset->descriptions[i]);
    }
    free(moveset->descriptions);
    free(moveset->operations);
}

int pl_moveset_accepts_sequence(PlMoveset * moveset, unsigned char * moves, int count) {
    if (count == 1) return 1;
    unsigned char lastMove = moves[count - 2];
    unsigned char currentMove = moves[count - 1];
    // filter redundancies if we can
    if (moveset->moveset == PlMovesetDefault) {
        if (!standard_operations_validate(moves, count)) {
            return 0;
        }
        if (!_pl_verify_faceturns(moveset, moves, count)) {
            return 0;
        }
    } else if (moveset->moveset == PlMovesetDouble) {
        if (currentMove == lastMove) return 0; // obvious redundancy
        if (lastMove % 2 == 1) {
            // on an odd move; this is where we limit it
            if (currentMove + 1 == lastMove) {
                // we don't perform a commuting even move from an odd move
                return 0;
            }
        }
    }
    
    return 1;
}

void pl_moveset_print_solution(PlMoveset * ms, unsigned char * moves, int numMoves) {
    int i;
    for (i = 0; i < numMoves; i++) {
        printf(" %s", ms->descriptions[moves[i]]);
    }
}

static int _pl_verify_faceturns(PlMoveset * ms, unsigned char * moves, int numMoves) {
    int independentFacesCount = ms->maxIndepFaces;
    if (independentFacesCount >= 6) return 1;
    char facesFlags = 0;
    int i;
    int numAxes = 0;
    int numCollisions = 0;
    // the number of allowed independent axes
    int allowedAxes = 2;
    if (independentFacesCount == 1) {
        allowedAxes = 1;
    } else if (independentFacesCount > 4) {
        allowedAxes = 3;
    }
    // the number of axes which can be shared
    int allowedCollisions = 3;
    if (independentFacesCount < 3) {
        allowedCollisions = 0;
    } else if (independentFacesCount == 3) {
        allowedCollisions = 1;
    } else if (independentFacesCount < 6) {
        allowedCollisions = 2;
    }
    for (i = 0; i < numMoves; i++) {
        char flag = 1 << (moves[i] % 6);
        // mask the other turn on this axis
        char otherFlag = flag;
        if ((moves[i] % 6) % 2 == 0) {
            otherFlag <<= 1;
        } else {
            otherFlag >>= 1;
        }
        if ((facesFlags & flag) != 0) continue;
        facesFlags |= flag;
        // if the other face on this axis has been used, this is a collision
        if ((facesFlags & otherFlag) != 0) {
            numCollisions++;
        } else {
            numAxes++;
        }
        if (numAxes > allowedAxes || numCollisions > allowedCollisions) {
            return 0;
        }
    }
    return 1;
}
