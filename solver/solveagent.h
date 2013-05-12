#ifndef __SOLVEAGENT_H__
#define __SOLVEAGENT_H__

#include <stdlib.h>
#include <unistd.h>

typedef struct {
    void (*initialize)(void * data);
    void (*destroy)(void * data);
    
    // create a group object
    void * (*create_group_identity)(void * data);
    // free a group object
    void (*free_group_object)(void * data, void * object);
    // perform an operation on a group object
    void (*operate)(void * data, void * destination, void * object, int operation);
    // detect if a group object is "solved"
    int (*is_goal)(void * data, void * object);
    // filter a sequence (usually a user feature for 3-gen algorithms)
    int (*accepts_sequence)(void * data, unsigned char * moves, int count);
    // returns 1 if the object cannot be solved in maxMoves or less
    int (*heuristic_exceeds)(void * data, void * object, int maxMoves);
    // report a solution once it has been accepted
    void (*report_solution)(void * data, unsigned char * moves, int count);
    // report the solve's progress
    void (*report_progress)(void * data, time_t duration, long long expandedCount, int depth);
    
    // basic arguments
    void * userData;
    void * startObject;
    int operationCount;
    int maxDepth;
    int minDepth;
    
    // user-specific flags
    int multipleSolutions;
    int verbosity; // 0 = none, 1 = print updates
    
    // user-specific arguments
    long long progressIncrement;
    int threadCount;
} SAUserInfo;

void sa_solve_main(SAUserInfo info);

#endif