#include "configcounter.h"
#include "subproblemindices.h"

int main(int argc, const char * argv[]) {
    const char * outputFile = NULL;
    int depth = 0, i;
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <output> <depth> <subproblem> [--operations=ALG1,ALG2,...]\n\
\n\nList of subproblems:\n\
- 'corners' - the corner subproblem\n\
- 'edgefront' - the front and top edge pieces\n\
- 'edgeback' - the back and bottom edge pieces\n\
- 'edgeall' - all of the twelve edges\n\
- 'eo' - all of the edges' orientations\n\
- 'block#' - 2x2x2 blocks numbered 0-7 (e.g. block3)\n\
- 'cross#' - the CFOP cross numbered 1-6 (e.g cross4)\n\
- 'R2F2U' - the subgroup <L2,R2,F2,B2,U,D>\n\
- 'edgeperm' - the position of the edges ignoring orientation\n", argv[0]);
        return 1;
    }
    outputFile = argv[1];
    depth = atoi(argv[2]);
    const char * subproblem = argv[3];

    CCUserInfo info;
    info.indexType = index_type_from_string(subproblem);
    info.shardDepth = 3; // provides a good split
    if (info.indexType == IndexTypeUnknown) {
        fprintf(stderr, "error: unknown subproblem %s\n", subproblem);
        exit(1);
    }
    RubiksMap * map = rubiks_map_new_identity();
    info.maximumDepth = depth;
    info.identity = map;
    pl_moveset_initialize(&info.moveset);
    
    for (i = 4; i < argc; i++) {
        if (parse_moveset_argument(&info.moveset, argv[i])) {
            continue;
        }
        fprintf(stderr, "error: unknown argument `%s`\n", argv[i]);
        exit(1);
    }
    
    ShardNode * node = cc_compute_table(info);
    rubiks_map_free(map);
    printf("Saving nodes...\n");
    
    FILE * fp = fopen(outputFile, "w");
    if (!fp) {
        perror("fopen");
        exit(1);
    }
    index_file_write(info.indexType,
                     index_type_data_size(info.indexType) + 1,
                     index_type_data_size(info.indexType) + 2,
                     info.maximumDepth, node, fp);
    fclose(fp);
    
    shard_node_free(node);
    pl_moveset_free(&info.moveset);
    return 0;
}
