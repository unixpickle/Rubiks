#include "cmdutil.h"

RubiksMap * parse_premoves_argument(const char * argument) {
    if (strlen(argument) > strlen("--premoves=")) {
        if (strncmp(argument, "--premoves=", strlen("--premoves=")) == 0) {
            const char * premoves = &argument[strlen("--premoves=")];
            return cube_for_algorithm(premoves);
        }
    }
    return NULL;
}

int parse_moveset_argument(PlMoveset * moveset, const char * argument) {
    if (strcmp(argument, "--double") == 0) {
        pl_moveset_free(moveset);
        pl_moveset_initialize_double(moveset);
        return 1;
    }
    if (strlen(argument) >= strlen("--maxfaces=")) {
        if (strncmp(argument, "--maxfaces=", strlen("--maxfaces=")) == 0) {
            if (moveset->moveset != PlMovesetDefault) {
                pl_moveset_free(moveset);
                pl_moveset_initialize(moveset);
            }
            moveset->maxIndepFaces = atoi(&argument[strlen("--maxfaces=")]);
            return 1;
        }
    }
    RubiksMap ** ops = NULL;
    char ** descs = NULL;
    int opCount;
    if ((opCount = parse_operations_argument(argument, &ops, &descs)) > 0) {
        pl_moveset_free(moveset);
        pl_moveset_initialize_custom(moveset, ops, descs, opCount);
        return 1;
    }
    return 0;
}

int parse_operations_argument(const char * argument, RubiksMap *** _maps, char *** _descriptions) {
    RubiksMap ** operations;
    char ** descriptions;
    const char * moveSet;
    
    if (strlen(argument) < strlen("--operations=")) return -1;
    if (strncmp(argument, "--operations", strlen("--operations")) != 0) {
        return -1;
    }
    moveSet = &argument[strlen("--operations=")];
    
    int numMaps = 1, i;
    for (i = 0; i < strlen(moveSet); i++) {
        if (moveSet[i] == ',') numMaps++;
    }
    operations = (RubiksMap **)malloc(sizeof(void *) * numMaps);
    descriptions = (char **)malloc(sizeof(char *) * numMaps);
    int index = 0;
    for (i = 0; i < strlen(moveSet) && index < numMaps; i++) {
        int algoLength = 0;
        for (algoLength = 0; algoLength < strlen(moveSet) - i; algoLength++) {
            if (moveSet[algoLength + i] == ',') break;
        }
        char * description = (char *)malloc(algoLength + 1);
        bzero(description, algoLength + 1);
        memcpy(description, &moveSet[i], algoLength);
        descriptions[index] = description;
        operations[index] = cube_for_algorithm(description);
        i += algoLength;
        index++;
    }
    *_maps = operations;
    *_descriptions = descriptions;
    return numMaps;
}
