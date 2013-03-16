#include "cube.h"

typedef int (*RubiksVerificationMethod)(RubiksMap * map);
void findSolutions(int length, RubiksMap ** operations, RubiksMap * input, RubiksVerificationMethod verifyMethod);
