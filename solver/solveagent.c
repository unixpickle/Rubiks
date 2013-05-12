#include "solveagent.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#define kDistributionBoundDepth 4

typedef struct {
    pthread_mutex_t lock;
    long long nodesExpanded;
    time_t startTime;
    int isCancelled;
} SASolveInfo;

typedef struct {
    int maxDepth;
    SAUserInfo * userInfo;
    SASolveInfo * solveInfo;
    
    unsigned char lowerBound[kDistributionBoundDepth];
    unsigned char upperBound[kDistributionBoundDepth];
    
    // used by thread
    void ** mapCache;
    char * lastMoves;
} SAThreadInfo;

static void sa_dispatch_threads(const char * boundStep, SAUserInfo * info,
                                int maxDepth, SASolveInfo * solve);
static void sa_dispatch_add_arbitrary(char * a1, const char * a2, int base);

static void * sa_solve_thread_main(void * _threadInfo);
static int sa_solve_recursive_method(SAThreadInfo * info, int currentDepth, void * object);
static void sa_cancel_solve(SAThreadInfo * info);

void sa_solve_main(SAUserInfo info) {
    // calculate the boundaries
    int i, theRemainder = 0;
    unsigned char baseBound[kDistributionBoundDepth];
    bzero(baseBound, kDistributionBoundDepth);
    baseBound[0] = info.operationCount / info.threadCount;
    theRemainder = info.operationCount % info.threadCount;
    for (i = 1; i < kDistributionBoundDepth && theRemainder != 0; i++) {
        int nextTotal = theRemainder * info.operationCount;
        baseBound[i] = nextTotal / info.threadCount;
        theRemainder = nextTotal % info.threadCount;
    }
    SASolveInfo solveInfo;
    solveInfo.startTime = time(NULL);
    solveInfo.nodesExpanded = 0;
    solveInfo.isCancelled = 0;
    pthread_mutex_init(&solveInfo.lock, NULL);
    
    if (info.initialize) {
        info.initialize(info.userData);
    }
    
    for (i = info.minDepth; i <= info.maxDepth; i++) {
        printf("Trying %d move solutions...\n", i);
        sa_dispatch_threads(baseBound, &info, i, &solveInfo);
    }
    
    if (info.destroy) {
        info.destroy(info.userData);
    }
    
    pthread_mutex_destroy(&solveInfo.lock);
}

static void sa_dispatch_threads(const char * boundStep, SAUserInfo * info,
                                int maxDepth, SASolveInfo * solve) {
    unsigned char startBound[kDistributionBoundDepth];
    unsigned char endBound[kDistributionBoundDepth];
    bzero(startBound, kDistributionBoundDepth);
    memcpy(endBound, boundStep, kDistributionBoundDepth);
    int i;
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) * info->threadCount);
    
    // printf("operation count = %d\n", info->operationCount);
    
    for (i = 0; i < info->threadCount; i++) {
        if (i == info->threadCount - 1) {
            int j;
            bzero(endBound, kDistributionBoundDepth);
            endBound[0] = info->operationCount;
        }
        
        
        /*printf("start bound (index %d): ", i);
        int j;
        for (j = 0; j < kDistributionBoundDepth; j++) {
            printf("%d ", startBound[j]);
        }
        
        printf("\nend bound (index %d): ", i);
        for (j = 0; j < kDistributionBoundDepth; j++) {
            printf("%d ", endBound[j]);
        }
        printf("\n");*/
        
        
        SAThreadInfo * state = (SAThreadInfo *)malloc(sizeof(SAThreadInfo));
        state->maxDepth = maxDepth;
        state->userInfo = info;
        state->solveInfo = solve;
        memcpy(state->lowerBound, startBound, kDistributionBoundDepth);
        memcpy(state->upperBound, endBound, kDistributionBoundDepth);
        pthread_create(&threads[i], NULL, &sa_solve_thread_main, state);
        
        memcpy(startBound, endBound, kDistributionBoundDepth);
        sa_dispatch_add_arbitrary(endBound, boundStep, info->operationCount);
        
        // DEBUGGING
        // pthread_join(threads[i], NULL);
    }
    for (i = 0; i < info->threadCount; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
}

static void sa_dispatch_add_arbitrary(char * a1, const char * a2, int base) {
    int i;
    int carry = 0;
    for (i = kDistributionBoundDepth - 1; i >= 0; i--) {
        a1[i] += carry;
        a1[i] += a2[i];
        if (a1[i] >= base) {
            a1[i] -= base;
            carry = 1;
        } else carry = 0;
    }
}

static void * sa_solve_thread_main(void * _threadInfo) {
    SAThreadInfo * threadInfo = (SAThreadInfo *)_threadInfo;
    int i;
    SAUserInfo * userInfo = threadInfo->userInfo;
    void * userData = userInfo->userData;
    
    // allocate resources for the solve search
    threadInfo->lastMoves = (char *)malloc(threadInfo->maxDepth);
    threadInfo->mapCache = (void **)malloc(sizeof(void *) * threadInfo->maxDepth);
    for (i = 0; i < threadInfo->maxDepth; i++) {
        threadInfo->mapCache[i] = userInfo->create_group_identity(userData);
    }
    
    sa_solve_recursive_method(threadInfo, 0, userInfo->startObject);
    
    // free everything
    for (i = 0; i < threadInfo->maxDepth; i++) {
        userInfo->free_group_object(userData, threadInfo->mapCache[i]);
    }
    free(threadInfo->mapCache);
    free(threadInfo->lastMoves);
    free(threadInfo);
}

static int sa_solve_recursive_method(SAThreadInfo * info, int currentDepth, void * object) {
    SAUserInfo * userInfo = info->userInfo;
    void * userData = userInfo->userData;
    
    // check for a goal state
    if (currentDepth == info->maxDepth) {
        /*printf("tried sequence: ");
        int j;
        for (j = 0; j < info->maxDepth; j++) {
            printf("%d ", info->lastMoves[j]);
        }
        printf("\n");*/
        if (userInfo->is_goal(userData, object)) {
            userInfo->report_solution(userData, info->lastMoves, currentDepth);
            if (!userInfo->multipleSolutions) {
                sa_cancel_solve(info);
                return 0;
            }
        }
        return 1;
    }
    
    // make sure we still have hope; if not, give up now!
    if (userInfo->heuristic_exceeds(userData, object, info->maxDepth - currentDepth)) {
        return 1;
    }
    
    int i, j;
    void * applicationDestination = info->mapCache[currentDepth];
    
    int lowerBound = 0;
    int upperBound = userInfo->operationCount - 1;
    if (currentDepth == 0) {
        lowerBound = info->lowerBound[0];
        upperBound = info->upperBound[0];
        int isOnlyDigit = 1;
        for (j = 1; j < kDistributionBoundDepth && j < info->maxDepth; j++) {
            if (info->upperBound[j] != 0) isOnlyDigit = 0;
        }
        if (isOnlyDigit) {
            upperBound -= 1;
        }
    } else if (currentDepth < kDistributionBoundDepth) {
        if (info->lastMoves[currentDepth - 1] == info->lowerBound[currentDepth - 1]) {
            lowerBound = info->lowerBound[currentDepth];
        }
        if (info->lastMoves[currentDepth - 1] == info->upperBound[currentDepth - 1]) {
            upperBound = info->upperBound[currentDepth];
            int isLastUpper = 1;
            for (j = currentDepth + 1; j < kDistributionBoundDepth && j < info->maxDepth; j++) {
                if (info->upperBound[j] > 0) {
                    isLastUpper = 0;
                }
            }
            if (isLastUpper) {
                upperBound -= 1;
            }
        }
    }
    
    for (i = lowerBound; i <= upperBound; i++) {
        info->lastMoves[currentDepth] = i;
        if (!userInfo->accepts_sequence(userData, info->lastMoves, currentDepth + 1)) {
            continue;
        }
        userInfo->operate(userData, applicationDestination, object, i);
        int result = sa_solve_recursive_method(info, currentDepth + 1,
                                               applicationDestination);
        if (!result) return 0;
    }
    
    return 1;
}

static void sa_cancel_solve(SAThreadInfo * threadInfo) {
    SASolveInfo * info = threadInfo->solveInfo;
    pthread_mutex_lock(&info->lock);
    info->isCancelled = 1;
    pthread_mutex_unlock(&info->lock);
}
