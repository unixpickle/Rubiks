#include <stdio.h>
#include "rubikspl_cmd.h"

void printUsage(const char * execName);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "rubiks") == 0) {
        rubikspl_cmd_main(argc - 1, &argv[1]);
    } else {
        fprintf(stderr, "error: unknown solver `%s`\n", argv[1]);
        return 1;
    }
    return 0;
}

void printUsage(const char * execName) {
    fprintf(stderr, "Usage: %s <solver> <arguments> ...\n\n", execName);
    fprintf(stderr, "List of solvers:\n\n");
    fprintf(stderr, " rubiks - standard 3x3x3 solver\n\n");
}
