#include "orientpl_cmd.h"

static void printUsage();
static void configure_user_info(SAUserInfo * info);

void orientpl_cmd_main(int argc, const char * argv[]) {
    int i;
    SAUserInfo userInfo;
    configure_user_info(&userInfo);
    OrientPl pluginInfo;
    pluginInfo.edgeHeuristic = NULL;
    orientpl_initialize(&pluginInfo);
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printUsage();
            goto freeAndReturn;
        } else if (strcmp(argv[i], "--multiple") == 0) {
            userInfo.multipleSolutions = 1;
            continue;
        } else {
            if (parse_solve_agent_argument(&userInfo, argv[i])) {
                continue;
            }
        }
        int result = parse_heuristic_argument(&pluginInfo.edgeHeuristic, argv[i]);
        if (result == 1) continue;
        if (result < 0) goto freeAndReturn;
        fprintf(stderr, "error: unknown option `%s`\n", argv[i]);
        goto freeAndReturn;
    }
    
    RubiksMap * map = rubiks_map_user_input();
    if (!map) {
        fprintf(stderr, "error: invalid scramble!\n");
        goto freeAndReturn;
    }
    
    userInfo.userData = &pluginInfo;
    userInfo.startObject = map;
    
    sa_solve_main(userInfo);
    
    rubiks_map_free(map);
    
freeAndReturn:
    orientpl_free(pluginInfo);
    if (pluginInfo.edgeHeuristic) {
        heuristic_table_free(pluginInfo.edgeHeuristic);
    }
    return;
}

static void printUsage() {
    fprintf(stderr, "Usage: orient [--help] [--heuristic=database] [--multiple]\n\
         [--mindepth=n] [--maxdepth=n] [--threads=n]\n");
}

static void configure_user_info(SAUserInfo * info) {
    bzero(info, sizeof(SAUserInfo));
    info->create_group_identity = orientpl_create_group_identity;
    info->free_group_object = orientpl_free_group_object;
    info->operate = orientpl_operate;
    info->is_goal = orientpl_is_goal;
    info->accepts_sequence = orientpl_accepts_sequence;
    info->heuristic_exceeds = orientpl_heuristic_exceeds;
    info->report_solution = orientpl_report_solution;
    info->report_progress = orientpl_report_progress;
    info->operationCount = 18;
    info->maxDepth = 7;
    info->minDepth = 0;
    info->multipleSolutions = 0;
    info->verbosity = 1;
    info->progressIncrement = 1 << 21;
    info->threadCount = 1;
}
