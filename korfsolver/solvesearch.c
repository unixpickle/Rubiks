#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "heuristic.h"
#include "cube.h"

typedef struct {
    int operationStart;
    int operationCount;
    int maxDepth;
    RubiksMap * map;
} ThreadState;

static pthread_mutex_t foundSolutionLock = PTHREAD_MUTEX_INITIALIZER;
static int foundSolutionFlag = 0;
static unsigned char solutionMovesData[20];
static int solutionMovesCount;

// command line arguments
static int findMultipleSolutions = 0;
static int independentFacesCount = 6;
static int startingDepth = 1;
static int doubleOnly = 0;

static RubiksMap ** operations;
static pthread_mutex_t nodesExpandedLock = PTHREAD_MUTEX_INITIALIZER;
static long long nodesExpanded = 0;

static int threadCount = 0;
static time_t startTime = 0;

static void dispatch_search_threads(RubiksMap * baseMap, int maxDepth);
static void * search_thread_main(void * ptr);
static int search_method_main(RubiksMap * baseMap, unsigned char * lastMoves, int currentDepth, int maxDepth, RubiksMap ** mapCache, long long * nodeCount);

static void thread_report_solved(const unsigned char * moves, int moveCount);
static int thread_should_return();

static int moves_fit_filters(unsigned char * moves, int numMoves);

int main(int argc, const char * argv[]) {
    int i;
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <back.anc2> <front.anc2> <corners.anc2> <max depth>\n\
         <thread count> [--multiple] [--faces_max=n] [--mindepth=n] [--double_only]\n", argv[0]);
        return 0;
    }
    // read the extra arguments
    for (i = 6; i < argc; i++) {
        if (strcmp(argv[i], "--multiple") == 0) {
            findMultipleSolutions = 1;
        } else if (strcmp(argv[i], "--double_only") == 0) {
            doubleOnly = 1;
        } else if (strncmp(argv[i], "--faces_max=", 12) == 0) {
            if (strlen(argv[i]) > 12) {
                independentFacesCount = atoi(&argv[i][12]);
            } else {
                fprintf(stderr, "warning: invalid --faces_max argument given\n");
            }
        } else if (strncmp(argv[i], "--mindepth=", 11) == 0) {
            if (strlen(argv[i]) > 11) {
                startingDepth = atoi(&argv[i][11]);
            } else {
                fprintf(stderr, "warning: invalid --mindepth argument given\n");
            }
        }
    }
    const char * backFile = argv[1];
    const char * frontFile = argv[2];
    const char * cornerFile = argv[3];
    int maxDepth = atoi(argv[4]);
    threadCount = atoi(argv[5]);
    if (threadCount == 0 || maxDepth == 0) {
        fprintf(stderr, "error: invalid maxDepth or threadCount\n");
        return 0;
    }
    if (!heuristic_load_index_files(cornerFile, frontFile, backFile)) {
        fprintf(stderr, "Failed to load index files.\n");
        return 0;
    }
    RubiksMap * userMap = rubiks_map_user_input();
    if (!userMap) {
        fprintf(stderr, "Invalid user input\n");
        return 0;
    }
    printf("\n\nNOTE: minimum moves: %d\n\n", heuristic_minimum_moves(userMap));
    if (rubiks_map_is_identity(userMap)) {
        rubiks_map_free(userMap);
        printf("You entered a solved cube.\n");
        return 0;
    }
    operations = cube_standard_face_turns();

    startTime = time(NULL);

    for (i = startingDepth; i <= maxDepth && i < 21; i++) {
        printf("Trying %d depth...\n", i);
        dispatch_search_threads(userMap, i);
        if (foundSolutionFlag) break;
    }
    
    for (i = 0; i < 18; i++) {
        rubiks_map_free(operations[i]);
    }
    rubiks_map_free(userMap);
    free(operations);
    heuristic_free_all();
    return 0;
}

static void dispatch_search_threads(RubiksMap * baseMap, int maxDepth) {
    if (threadCount > 18) threadCount = 18;
    int threadOperations = 18 / threadCount;
    int i;
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) * threadCount);
    for (i = 0; i < threadCount; i++) {
        int operationIndex = i * threadOperations;
        int operationCount = i == threadCount - 1 ? 18 - operationIndex : threadOperations;
        ThreadState * state = (ThreadState *)malloc(sizeof(ThreadState));
        state->map = baseMap;
        state->operationCount = operationCount;
        state->operationStart = operationIndex;
        state->maxDepth = maxDepth;
        pthread_create(&threads[i], NULL, &search_thread_main, state);
    }
    for (i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
}

static void * search_thread_main(void * ptr) {
    ThreadState * state = (ThreadState *)ptr;
    RubiksMap * map = state->map;
    int maxDepth = state->maxDepth;
    int operationStart = state->operationStart;
    int operationCount = state->operationCount;
    free(state);
    // perform first iteration here, then dispatch our main moves.
    if (rubiks_map_is_identity(map)) {
        thread_report_solved(NULL, 0);
        return NULL;
    }
    if (maxDepth == 0) return NULL;
    int i;
    RubiksMap ** mapCache = (RubiksMap **)malloc(sizeof(RubiksMap *) * maxDepth);
    for (i = 0; i < maxDepth; i++) {
        mapCache[i] = rubiks_map_new_identity();
    }
    unsigned char lastMoves[24];
    long long nodeCount = 0;
    for (i = 0; i < operationCount; i++) {
        lastMoves[0] = i + operationStart;
        RubiksMap * operation = operations[i + operationStart];
        rubiks_map_multiply(mapCache[0], operation, map);
        search_method_main(mapCache[0], lastMoves, 1, maxDepth, mapCache, &nodeCount);
        if (thread_should_return()) break;
    }
    for (i = 0; i < maxDepth; i++) {
        rubiks_map_free(mapCache[i]);
    }
    free(mapCache);
    pthread_mutex_lock(&nodesExpandedLock);
    nodesExpanded += nodeCount;
    pthread_mutex_unlock(&nodesExpandedLock);
    return NULL;
}

static int search_method_main(RubiksMap * baseMap, unsigned char * previousMoves, int currentDepth, int maxDepth, RubiksMap ** mapCache, long long * nodeCount) {
    nodeCount[0] += 1;
    if (nodeCount[0] > (1<<21)) {
        pthread_mutex_lock(&nodesExpandedLock);
        nodesExpanded += nodeCount[0];
        printf("Expanded %lld nodes [depth = %d, time = %lld sec]\n",
               nodesExpanded, maxDepth, (long long)(time(NULL) - startTime));
        pthread_mutex_unlock(&nodesExpandedLock);
        nodeCount[0] = 0;
    } else if (nodeCount[0] % (1<<18) == 0) { // periodically check
        if (thread_should_return()) return 1;
    }
    if (currentDepth == maxDepth) {
        if (rubiks_map_is_identity(baseMap)) {
            if (moves_fit_filters(previousMoves, currentDepth)) {
                thread_report_solved(previousMoves, currentDepth);
                if (!findMultipleSolutions) return 1;
            }
        }
        return 0;
    }
    if (heuristic_exceeds_moves(baseMap, maxDepth - currentDepth)) return 0;
    int i;
    RubiksMap * map = mapCache[currentDepth];
    char lastMove = previousMoves[currentDepth - 1];
    for (i = 0; i < 18; i++) {
        if (lastMove % 2 == 1) {
            // on an odd move; this is where we limit it
            if ((i % 6) + 1 == lastMove % 6) {
                // we don't perform a commuting even move from an odd move
                continue;
            }
        }
        if (i == lastMove) continue;
        if (i % 6 == lastMove % 6) {
            // we are making another turn on the same face :(
            continue;
        }
        previousMoves[currentDepth] = (unsigned char)i;
        rubiks_map_multiply(map, operations[i], baseMap);
        if (search_method_main(map, previousMoves, currentDepth + 1, maxDepth, mapCache, nodeCount)) {
            return 1;
        }
    }
    return 0;
}

static void thread_report_solved(const unsigned char * moves, int moveCount) {
    pthread_mutex_lock(&foundSolutionLock);
    printf("Found solution: ");
    cube_print_standard_solution(moves, moveCount);
    printf(" [%lld seconds]\n", (long long)(time(NULL) - startTime));
    if (findMultipleSolutions) {
        pthread_mutex_unlock(&foundSolutionLock);
        return;
    }
    if (!foundSolutionFlag) {
        foundSolutionFlag = 1;
        if (moveCount > 0) memcpy(solutionMovesData, moves, moveCount);
        solutionMovesCount = moveCount;
    }
    pthread_mutex_unlock(&foundSolutionLock);
}

static int thread_should_return() {
    if (findMultipleSolutions) return 0;
    int flag;
    pthread_mutex_lock(&foundSolutionLock);
    flag = foundSolutionFlag;
    pthread_mutex_unlock(&foundSolutionLock);
    return flag;
}

static int moves_fit_filters(unsigned char * moves, int numMoves) {
    if (doubleOnly) {
        int i;
        for (i = 0; i < numMoves; i++) {
            if (moves[i] < 12) return 0;
        }
    }
    if (independentFacesCount >= 6) return 1;
    // check the axis of every move and count how many independent faces/axes there are
    char facesFlags = 0;
    int i;
    int numAxes = 0;
    int numCollisions = 0;
    // the number of allowed independent axes
    int allowedAxes = 2;
    if (independentFacesCount == 1) {
        allowedAxes = 1;
    } else if (independentFacesCount > 4) {
        allowedAxes = 3;
    }
    // the number of axes which can be shared
    int allowedCollisions = 3;
    if (independentFacesCount < 3) {
        allowedCollisions = 0;
    } else if (independentFacesCount == 3) {
        allowedCollisions = 1;
    } else if (independentFacesCount < 6) {
        allowedCollisions = 2;
    }
    for (i = 0; i < numMoves; i++) {
        char flag = 1 << (moves[i] % 6);
        // mask the other turn on this axis
        char otherFlag = flag;
        if ((moves[i] % 6) % 2 == 0) {
            otherFlag <<= 1;
        } else {
            otherFlag >>= 1;
        }
        if ((facesFlags & flag) != 0) continue;
        facesFlags |= flag;
        // if the other face on this axis has been used, this is a collision
        if ((facesFlags & otherFlag) != 0) {
            numCollisions++;
        } else {
            numAxes++;
        }
        if (numAxes > allowedAxes || numCollisions > allowedCollisions) {
            return 0;
        }
    }
    return 1;
}
