#include "rubikspl_cmd.h"

static void printUsage();
static void generate_user_info(SAUserInfo * info);

void rubikspl_cmd_main(int argc, const char * argv[]) {
    SAUserInfo solveUserInfo;
    RubiksPl pluginInfo;
    RubiksMap * premoves = NULL;
    pluginInfo.heuristics = heuristic_list_new();
    pl_moveset_initialize(&pluginInfo.moveset);
    generate_user_info(&solveUserInfo);
    solveUserInfo.userData = &pluginInfo;
    
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printUsage();
            goto freeAndReturn;
        } else if (strcmp(argv[i], "--heuristic") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "warning: --heuristic passed with no heuristic file.\n");
            } else {
                const char * file = argv[i + 1];
                i++;
                HeuristicTable * table = heuristic_table_load(file);
                if (!table) {
                    fprintf(stderr, "Failed to load heuristic: %s\n", file);
                    goto freeAndReturn;
                }
                heuristic_list_add(pluginInfo.heuristics, table);
            }
            continue;
        } else if (strcmp(argv[i], "--multiple") == 0) {
            solveUserInfo.multipleSolutions = 1;
            continue;
        }
        RubiksMap * possibleMap = NULL;
        if (possibleMap = parse_premoves_argument(argv[i])) {
            premoves = possibleMap;
            continue;
        }
        // numerical arguments 
        if (parse_solve_agent_argument(&solveUserInfo, argv[i])) {
            continue;
        }
        // moveset arguments
        if (parse_moveset_argument(&pluginInfo.moveset, argv[i])) {
            continue;
        }
        fprintf(stderr, "error: unknown option `%s`\n", argv[i]);
        goto freeAndReturn;
    }
    
    solveUserInfo.operationCount = pluginInfo.moveset.operationCount;
    
    RubiksMap * scramble = cube_validated_user_input(premoves);
    if (!scramble) {
        fprintf(stderr, "error: failed to read scramble!\n");
        goto freeAndReturn;
    }
    
    solveUserInfo.startObject = scramble;
    sa_solve_main(solveUserInfo);
    
    rubiks_map_free(scramble);
    
freeAndReturn:
    rubikspl_context_free(pluginInfo);
    if (premoves) rubiks_map_free(premoves);
}

static void printUsage() {
    fprintf(stderr, "Usage: rubiks [--help] [--heuristic <file>] [--multiple] [--threads=n]\n\
        [--premoves=ALG] [--mindepth=n] [--maxdepth=n]\n\
        [--double | --maxfaces=n | --operations=alg1,alg2,...]\n");
}

static void generate_user_info(SAUserInfo * info) {
    bzero(info, sizeof(SAUserInfo));
    info->operationCount = 18;
    info->multipleSolutions = 0;
    info->verbosity = 1;
    info->progressIncrement = 1 << 21;
    info->threadCount = 6;
    info->create_group_identity = rubikspl_create_group_identity;
    info->free_group_object = rubikspl_free_group_object;
    info->operate = rubikspl_operate;
    info->is_goal = rubikspl_is_goal;
    info->accepts_sequence = rubikspl_accepts_sequence;
    info->heuristic_exceeds = rubikspl_heuristic_exceeds;
    info->report_solution = rubikspl_report_solution;
    info->report_progress = rubikspl_report_progress;
    info->minDepth = 0;
    info->maxDepth = 20;
}
