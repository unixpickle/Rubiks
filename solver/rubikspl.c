#include "rubikspl.h"

static int _rubikspl_verify_faceturns(RubiksPl * pl, unsigned char * moves, int numMoves);

void rubikspl_context_free(RubiksPl pl) {
    int i;
    for (i = 0; i < pl.operationCount; i++) {
        rubiks_map_free(pl.operations[i]);
        free(pl.descriptions[i]);
    }
    free(pl.descriptions);
    free(pl.operations);
}

void rubikspl_context_new_default(RubiksPl * pl) {
    const char * names[] = {
        "U", "D'", "R'", "L", "F", "B'",
        "U'", "D", "R", "L'", "F'", "B",
        "U2", "D2", "R2", "L2", "F2", "B2"
    };
    
    pl->operations = cube_standard_face_turns();
    pl->descriptions = (char **)malloc(sizeof(char *) * 18);
    int i;
    for (i = 0; i < 18; i++) {
        char * str = (char *)malloc(strlen(names[i]) + 1);
        strcpy(str, names[i]);
        pl->descriptions[i] = str;
    }
    pl->operationCount = 18;
    pl->moveset = RubiksPlMovesetDefault;
    pl->maxIndepFaces = 6;
}

void rubikspl_context_new_double(RubiksPl * pl) {
    const char * moves[] = {
        "F2", "B2", "U2", "D2", "R2", "L2"
    };
    pl->operations = (RubiksMap **)malloc(sizeof(void *) * 6);
    pl->descriptions = (char **)malloc(sizeof(char *) * 6);
    int i;
    for (i = 0; i < 6; i++) {
        pl->operations[i] = cube_for_algorithm(moves[i]);
        pl->descriptions[i] = (char *)malloc(strlen(moves[i]) + 1);
        strcpy(pl->descriptions[i], moves[i]);
    }
    pl->operationCount = 6;
    pl->moveset = RubiksPlMovesetDouble;
    pl->maxIndepFaces = 6;
}

void rubikspl_context_new_custom(RubiksPl * pl, const char * moveSet) {
    int numMaps = 1, i;
    for (i = 0; i < strlen(moveSet); i++) {
        if (moveSet[i] == ',') numMaps++;
    }
    pl->operations = (RubiksMap **)malloc(sizeof(void *) * numMaps);
    pl->descriptions = (char **)malloc(sizeof(char *) * numMaps);
    int index = 0;
    for (i = 0; i < strlen(moveSet) && index < numMaps; i++) {
        int algoLength = 0;
        for (algoLength = 0; algoLength < strlen(moveSet) - i; algoLength++) {
            if (moveSet[algoLength + i] == ',') break;
        }
        char * description = (char *)malloc(algoLength + 1);
        bzero(description, algoLength + 1);
        memcpy(description, &moveSet[i], algoLength);
        pl->descriptions[index] = description;
        pl->operations[index] = cube_for_algorithm(description);
        i += algoLength;
        index++;
    }
    pl->operationCount = numMaps;
    pl->moveset = RubiksPlMovesetCustom;
}

// called by solver agent

void * rubikspl_create_group_identity(void * data) {
    return rubiks_map_new_identity();
}

void rubikspl_free_group_object(void * data, void * object) {
    rubiks_map_free((RubiksMap *)object);
}

void rubikspl_operate(void * data, void * destination, void * object, int operation) {
    RubiksPl * pl = (RubiksPl *)data;
    rubiks_map_multiply(destination, pl->operations[operation], object);
}

int rubikspl_is_goal(void * data, void * object) {
    return rubiks_map_is_identity(object);
}

int rubikspl_accepts_sequence(void * data, unsigned char * moves, int count) {
    RubiksPl * pl = (RubiksPl *)data;
    if (count == 1) return 1;
    unsigned char lastMove = moves[count - 2];
    unsigned char currentMove = moves[count - 1];
    // filter redundancies if we can
    if (pl->moveset == RubiksPlMovesetDefault) {
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
        
        if (!_rubikspl_verify_faceturns(pl, moves, count)) {
            return 0;
        }
    } else if (pl->moveset == RubiksPlMovesetDouble) {
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

int rubikspl_heuristic_exceeds(void * data, void * object, int maxMoves) {
    RubiksPl * pl = (RubiksPl *)data;
    return heuristic_list_exceeds(pl->heuristics, (RubiksMap *)object, maxMoves);
}

void rubikspl_report_solution(void * data, unsigned char * moves, int count) {
    RubiksPl * pl = (RubiksPl *)data;
    printf("Found solution:");
    int i;
    for (i = 0; i < count; i++) {
        printf(" %s", pl->descriptions[moves[i]]);
    }
    printf("\n");
}

void rubikspl_report_progress(void * data, time_t duration, long long expandedCount, int depth) {
    printf("Expanded %lld nodes [depth = %d, time = %lld seconds]\n", 
           expandedCount, depth, (long long)duration);
}

static int _rubikspl_verify_faceturns(RubiksPl * pl, unsigned char * moves, int numMoves) {
    int independentFacesCount = pl->maxIndepFaces;
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
