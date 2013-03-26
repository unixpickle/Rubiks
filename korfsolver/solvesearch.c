#include "heuristic.h"
#include "cube.h"
#include <pthread.h>
#include <stdio.h>

#define kSolveSearchThreadCount 4

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

static RubiksMap ** operations;
static pthread_mutex_t nodesExpandedLock = PTHREAD_MUTEX_INITIALIZER;
static long long nodesExpanded = 0;

static void dispatch_search_threads(RubiksMap * baseMap, int maxDepth);
static void * search_thread_main(void * ptr);
static int search_method_main(RubiksMap * baseMap, unsigned char * lastMoves, int currentDepth, int maxDepth, RubiksMap ** mapCache, long long * nodeCount);

static void thread_report_solved(const unsigned char * moves, int moveCount);
static int thread_should_return();

int main(int argc, const char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <back.anci> <front.anci> <corners.anci> <max depth>\n", argv[0]);
        return 0;
    }
    const char * backFile = argv[1];
    const char * frontFile = argv[2];
    const char * cornerFile = argv[3];
    int maxDepth = atoi(argv[4]);
    if (!heuristic_load_index_files(cornerFile, frontFile, backFile)) {
        fprintf(stderr, "Failed to load index files.\n");
        return 0;
    }
    RubiksMap * userMap = cube_user_input();
    if (!userMap) {
        fprintf(stderr, "Invalid user input\n");
        return 0;
    }
    printf("\n\nNOTE: minimum moves: %d\n\n", heuristic_minimum_moves(userMap));
    if (cube_is_solved(userMap)) {
        rubiks_map_free(userMap);
        printf("You entered a solved cube.\n");
        return 0;
    }
    operations = cube_standard_face_turns();

    cube_print_solution_key();
    printf("\n");

    int i;
    for (i = 1; i <= maxDepth && i < 21; i++) {
        printf("Trying %d depth...\n", i);
        dispatch_search_threads(userMap, i);
        if (foundSolutionFlag) {
            printf("Found solution: ");
            int j;
            for (j = 0; j < solutionMovesCount; j++) {
                printf("%d ", solutionMovesData[j]);
            }
            printf("\n");
            foundSolutionFlag = 0;
        }
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
    int threadCount = kSolveSearchThreadCount;
    if (threadCount > 18) threadCount = 0;
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
    if (cube_is_solved(map)) {
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
        printf("Expanded %lld nodes [depth = %d]\n", nodesExpanded, maxDepth);
        pthread_mutex_unlock(&nodesExpandedLock);
        nodeCount[0] = 0;
    }
    if (currentDepth == maxDepth) {
        if (cube_is_solved(baseMap)) {
            thread_report_solved(previousMoves, currentDepth);
            return 1;
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
        if (lastMove >= 12 && i == lastMove) continue;
        if (lastMove < 12 && i % 6 == lastMove % 6 && i != lastMove) {
            // we are making an inverse of the previous move
            continue;
        }
        previousMoves[currentDepth] = (unsigned char)i;
        rubiks_map_multiply(map, operations[i], baseMap);
        if (search_method_main(map, previousMoves, currentDepth + 1, maxDepth, mapCache, nodeCount)) {
            return 1;
        }
    }
    if (thread_should_return()) return 1;
    return 0;
}

static void thread_report_solved(const unsigned char * moves, int moveCount) {
    pthread_mutex_lock(&foundSolutionLock);
    if (!foundSolutionFlag) {
        foundSolutionFlag = 1;
        if (moveCount > 0) memcpy(solutionMovesData, moves, moveCount);
        solutionMovesCount = moveCount;
    }
    pthread_mutex_unlock(&foundSolutionLock);
}

static int thread_should_return() {
    int flag;
    pthread_mutex_lock(&foundSolutionLock);
    flag = foundSolutionFlag;
    pthread_mutex_unlock(&foundSolutionLock);
    return flag;
}

