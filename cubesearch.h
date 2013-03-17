#ifndef __CUBE_SEARCH_H__
#define __CUBE_SEARCH_H__

#include "cube.h"

typedef int (*RubiksVerificationMethod)(RubiksMap * map);
void findSolutions(int length, RubiksMap ** operations, RubiksMap * input, RubiksVerificationMethod verifyMethod);

#endif
