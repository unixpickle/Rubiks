#include "cmdutil.h"

int parse_solve_agent_argument(SAUserInfo * info, const char * argument) {
    if (strlen(argument) > strlen("--threads=")) {
        if (strncmp(argument, "--threads=", strlen("--threads=")) == 0) {
            info->threadCount = atoi(&argument[strlen("--threads=")]);
            if (info->threadCount <= 0) info->threadCount = 1;
            return 1;
        }
    }
    if (strlen(argument) > strlen("--mindepth=")) {
        if (strncmp(argument, "--mindepth=", strlen("--mindepth=")) == 0) {
            info->minDepth = atoi(&argument[strlen("--mindepth=")]);
            if (info->minDepth < 0) info->minDepth = 0;
            return 1;
        } else if (strncmp(argument, "--maxdepth=", strlen("--maxdepth=")) == 0) {
            info->maxDepth = atoi(&argument[strlen("--maxdepth=")]);
            if (info->maxDepth < 0) info->maxDepth = 0;
            return 1;
        }
    }
    return 0;
}

int parse_heuristic_argument(HeuristicTable ** out, const char * argument) {
    if (strlen(argument) > strlen("--heuristic=")) {
        if (strncmp(argument, "--heuristic=", strlen("--heuristic=")) == 0) {
            const char * path = &argument[strlen("--heuristic=")];
            if (*out) {
                heuristic_table_free(*out);
            }
            *out = heuristic_table_load(path);
            if (!*out) {
                fprintf(stderr, "Failed to read heuristic file: %s\n", path);
                return -1;
            }
            return 1;
        }
    }
    return 0;
}

RubiksMap * parse_premoves_argument(const char * argument) {
    if (strlen(argument) > strlen("--premoves=")) {
        if (strncmp(argument, "--premoves=", strlen("--premoves=")) == 0) {
            const char * premoves = &argument[strlen("--premoves=")];
            return cube_for_algorithm(premoves);
        }
    }
    return NULL;
}
