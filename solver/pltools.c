#include "pltools.h"

int standard_operations_validate(const char * moves, int count) {
    if (count == 1) return 1;
    unsigned char lastMove = moves[count - 2];
    unsigned char currentMove = moves[count - 1];
    if (lastMove % 2 == 1) {
        // on an odd move; this is where we limit it
        if ((currentMove % 6) + 1 == lastMove % 6) {
            // we don't perform a commuting even move from an odd move
            return 0;
        }
    }
    if (currentMove == lastMove) return 0;
    if (currentMove % 6 == lastMove % 6) {
        // we are making another turn on the same face :(
        return 0;
    }
    return 1;
}