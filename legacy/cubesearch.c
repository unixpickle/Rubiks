#include "cubesearch.h"

#include <pthread.h>
#include <stdlib.h>

#define kCubeSearchThreadCount 4
#define kCubeSearchOperationCount 18

typedef struct {
    int done;
    pthread_mutex_t lock;
} SyncStructure;

typedef struct {
    int length;
    int segmentIndex;
    int segmentLength;
    RubiksMap ** operations;
    RubiksMap * input;
    RubiksVerificationMethod verifyMethod;
    int threadIndex;
    SyncStructure * sync;
} ThreadStructure;

static void * _solution_search_method(void * argument);
static int _solution_search_is_done(SyncStructure * st);

void findSolutions(int length, RubiksMap ** operations, RubiksMap * input, RubiksVerificationMethod verifyMethod) {
    printf("Searching for %d-move solutions\n", length);
    SyncStructure sync;
    bzero(&sync, sizeof(sync));
    pthread_mutex_init(&sync.lock, NULL);
    int threadCount = kCubeSearchThreadCount;
    if (threadCount > kCubeSearchOperationCount) {
        threadCount = kCubeSearchOperationCount;
    }
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) * threadCount);
    int i;
    ThreadStructure generalInput;
    generalInput.length = length;
    generalInput.operations = operations;
    generalInput.input = input;
    generalInput.verifyMethod = verifyMethod;
    generalInput.sync = &sync;
    for (i = 0; i < threadCount; i++) {
        int operationCount = kCubeSearchOperationCount / threadCount;
        if (i == threadCount - 1) {
            operationCount += kCubeSearchOperationCount % threadCount;
        }
        ThreadStructure * threadInput = (ThreadStructure *)malloc(sizeof(ThreadStructure));
        memcpy(threadInput, &generalInput, sizeof(generalInput));
        threadInput->threadIndex = i;
        threadInput->segmentIndex = i * (kCubeSearchOperationCount / threadCount);
        threadInput->segmentLength = operationCount;
        pthread_create(&threads[i], NULL, &_solution_search_method, threadInput);
    }
    for (i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    pthread_mutex_destroy(&sync.lock);
}

static void * _solution_search_method(void * _argument) {
    ThreadStructure * argument = (ThreadStructure *)_argument;
    // extract arguments
    int length = argument->length;
    int segmentIndex = argument->segmentIndex;
    int segmentLength = argument->segmentLength;
    RubiksMap ** operations = argument->operations;
    RubiksMap * input = argument->input;
    RubiksVerificationMethod verifyMethod = argument->verifyMethod;
    int threadIndex = argument->threadIndex;
    SyncStructure * sync = argument->sync;
    free(argument);

    int * indices = (int *)malloc(sizeof(int) * length);
    bzero(indices, sizeof(int) * length);
    indices[length - 1] = segmentIndex;
    int i, done = 0;
    RubiksMap * result = rubiks_map_new_identity();
    RubiksMap * tmp = rubiks_map_new_identity();

    int iterationTracker = 0;
    while (!done) {
        iterationTracker++;
        if (iterationTracker >= 10000) {
            iterationTracker = 0;
            pthread_mutex_lock(&sync->lock);
            int shouldTerminate = sync->done;
            void * addr = &sync->done;
            pthread_mutex_unlock(&sync->lock);
            if (shouldTerminate) {
                done = 1;
                break;
            }
        }
        rubiks_map_copy_into(result, input);
        for (i = 0; i < length; i++) {
            rubiks_map_copy_into(tmp, result);
            rubiks_map_multiply(result, operations[indices[i]], tmp);
        }
        if (verifyMethod(result)) {
            // TODO: there is a TINY possible synchronization issue here...
            int shouldTerminate;
            pthread_mutex_lock(&sync->lock);
            shouldTerminate = sync->done;
            sync->done = 1;
            pthread_mutex_unlock(&sync->lock);
            if (!shouldTerminate) {
                cube_print_standard_solution(indices, length);
            }
            break;
        }
        for (i = 0; i < length; i++) {
            indices[i]++;
            if (i == length - 1) {
                if (indices[i] >= segmentIndex + segmentLength) done = 1;
                break;
            }
            if (indices[i] >= kCubeSearchOperationCount) indices[i] = 0;
            else break;
        }
    }
    // free all of our state information
    free(indices);
    rubiks_map_free(result);
    rubiks_map_free(tmp);
}

static int _solution_search_is_done(SyncStructure * sync) {
    int shouldTerminate = 0;
    pthread_mutex_lock(&sync->lock);
    shouldTerminate = sync->done;
    pthread_mutex_unlock(&sync->lock);
}

