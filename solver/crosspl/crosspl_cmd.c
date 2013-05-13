#include "crosspl_cmd.h"

static void printUsage();
static void configure_user_info(SAUserInfo * info);

void crosspl_cmd_main(int argc, const char * argv[]) {
    if (argc < 2) {
        printUsage();
        return;
    }
    int i;
    SAUserInfo userInfo;
    configure_user_info(&userInfo);
    CrossPl pluginInfo;
    pluginInfo.heuristic = NULL;
    pluginInfo.crossIndex = atoi(argv[1]) - 1;
    if (pluginInfo.crossIndex < 0 || pluginInfo.crossIndex > 5) {
        fprintf(stderr, "error: invalid cross index `%s`\n", argv[1]);
        return;
    }
    crosspl_initialize(&pluginInfo);
    for (i = 2; i < argc; i++) {
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
        int result = parse_heuristic_argument(&pluginInfo.heuristic, argv[i]);
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
    crosspl_free(pluginInfo);
    if (pluginInfo.heuristic) {
        heuristic_table_free(pluginInfo.heuristic);
    }
    return;
}

static void printUsage() {
    fprintf(stderr, "Usage: cross <number 1-6> [--help] [--heuristic=database] [--multiple]\n\
         [--mindepth=n] [--maxdepth=n] [--threads=n]\n");
}

static void configure_user_info(SAUserInfo * info) {
    bzero(info, sizeof(SAUserInfo));
    info->create_group_identity = crosspl_create_group_identity;
    info->free_group_object = crosspl_free_group_object;
    info->operate = crosspl_operate;
    info->is_goal = crosspl_is_goal;
    info->accepts_sequence = crosspl_accepts_sequence;
    info->heuristic_exceeds = crosspl_heuristic_exceeds;
    info->report_solution = crosspl_report_solution;
    info->report_progress = crosspl_report_progress;
    info->operationCount = 18;
    info->maxDepth = 8;
    info->minDepth = 0;
    info->multipleSolutions = 0;
    info->verbosity = 1;
    info->progressIncrement = 1 << 21;
    info->threadCount = 1;
}
