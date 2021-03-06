#ifndef __CUBE_H__
#define __CUBE_H__

#include "rubiksmap.h"
#include "math/parity.h"
#include <stdio.h>

RubiksMap * cube_validated_user_input(RubiksMap * premoves);
RubiksMap ** cube_standard_face_turns();
void cube_print_standard_solution(const unsigned char * moves, int length);
RubiksMap * cube_for_algorithm(const char * algorithm);

#endif
