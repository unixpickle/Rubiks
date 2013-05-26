#include "r2f2upl_cmd.h"

static void printUsage();
static void configure_user_info(SAUserInfo * info);

void r2f2upl_cmd_main(int argc, const char * argv[]) {
    int i;
    SAUserInfo userInfo;
    configure_user_info(&userInfo);
    R2F2UPl pluginInfo;
    pluginInfo.heuristic = NULL;
    r2f2upl_initialize(&pluginInfo);
    
    RubiksMap * premoves = NULL;
    
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
        int result = parse_heuristic_argument(&pluginInfo.heuristic, argv[i]);
        if (result == 1) continue;
        if (result < 0) goto freeAndReturn;
        if (parse_moveset_argument(&pluginInfo.moveset, argv[i])) {
            continue;
        }
        RubiksMap * possibleMap = NULL;
        possibleMap = parse_premoves_argument(argv[i]);
        if (possibleMap) {
            premoves = possibleMap;
            continue;
        }
        fprintf(stderr, "error: unknown option `%s`\n", argv[i]);
        goto freeAndReturn;
    }
        
    userInfo.operationCount = pluginInfo.moveset.operationCount;
    
    RubiksMap * map = rubiks_map_user_input_premoves(premoves);
    if (!map) {
        fprintf(stderr, "error: invalid scramble!\n");
        goto freeAndReturn;
    }
    
    userInfo.userData = &pluginInfo;
    userInfo.startObject = map;
    
    sa_solve_main(userInfo);
    
    rubiks_map_free(map);
    
freeAndReturn:
    r2f2upl_free(pluginInfo);
    if (premoves) rubiks_map_free(premoves);
}

static void printUsage() {
    fprintf(stderr, "Usage: r2f2u [--help] [--heuristic=database] [--multiple]\n\
         [--mindepth=n] [--maxdepth=n] [--threads=n]\n");
}

static void configure_user_info(SAUserInfo * info) {
    bzero(info, sizeof(SAUserInfo));
    info->create_group_identity = r2f2upl_create_group_identity;
    info->free_group_object = r2f2upl_free_group_object;
    info->operate = r2f2upl_operate;
    info->is_goal = r2f2upl_is_goal;
    info->accepts_sequence = r2f2upl_accepts_sequence;
    info->heuristic_exceeds = r2f2upl_heuristic_exceeds;
    info->report_solution = r2f2upl_report_solution;
    info->report_progress = r2f2upl_report_progress;
    info->operationCount = 18;
    info->maxDepth = 12;
    info->minDepth = 0;
    info->multipleSolutions = 0;
    info->verbosity = 1;
    info->progressIncrement = 1 << 21;
    info->threadCount = 1;
}
