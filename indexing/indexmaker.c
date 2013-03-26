#include "configcounter.h"
#include "subproblemindices.h"

int main(int argc, const char * argv[]) {
    const char * outputFile = NULL;
    int depth = 0;
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <output> <depth> <subproblem>\n\
\n\nList of subproblems:\n\
- 'corners' - the corner subproblem\n\
- 'edgefront' - the front and top edge pieces\n\
- 'edgeback' - the back and bottom edge pieces\n\
- 'edgeall' - all of the twelve edges\n\n", argv[0]);
        return 1;
    }
    outputFile = argv[1];
    depth = atoi(argv[2]);
    const char * subproblem = argv[3];

    CCUserInfo info;
    info.shardDepth = 5; // provides a good split
    info.indexType = index_type_from_string(subproblem);
    if (info.indexType == IndexTypeUnknown) {
        fprintf(stderr, "error: unknown subproblem %s\n", subproblem);
    }
    RubiksMap * map = rubiks_map_new_identity();
    info.maximumDepth = depth;
    info.identity = map;
    ShardNode * node = cc_compute_table(info);
    rubiks_map_free(map);
    printf("Saving nodes...\n");
    
    FILE * fp = fopen(outputFile, "w");
    index_file_write(info.indexType,
                     index_type_data_size(info.indexType) + 1,
                     index_type_data_size(info.indexType) + 2,
                     info.maximumDepth, node, fp);
    fclose(fp);
    
    shard_node_free(node);
    return 0;
}
