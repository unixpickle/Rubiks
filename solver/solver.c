#include <stdio.h>
#include "rubikspl/rubikspl_cmd.h"
#include "orientpl/orientpl_cmd.h"
#include "blockpl/blockpl_cmd.h"
#include "crosspl/crosspl_cmd.h"
#include "r2f2upl/r2f2upl_cmd.h"

void printUsage(const char * execName);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "rubiks") == 0) {
        rubikspl_cmd_main(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "orient") == 0) {
        orientpl_cmd_main(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "block") == 0) {
        blockpl_cmd_main(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "cross") == 0) {
        crosspl_cmd_main(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "r2f2u") == 0) {
        r2f2upl_cmd_main(argc - 1, &argv[1]);
    } else {
        fprintf(stderr, "error: unknown solver `%s`\n", argv[1]);
        return 1;
    }
    return 0;
}

void printUsage(const char * execName) {
    fprintf(stderr, "Usage: %s <solver> <arguments> ...\n\n", execName);
    fprintf(stderr, "List of solvers:\n\n");
    fprintf(stderr, " rubiks - standard 3x3x3 solver [GN=20]\n\
 orient - orients all the edges on a 3x3x3 [GN=7]\n\
 block - solves a 2x2x2 block on a 3x3x3 [GN=8]\n\
 cross - solves a CFOP cross [GN=8]\n\n");
}
