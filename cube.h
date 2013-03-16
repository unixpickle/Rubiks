#ifndef __CUBE_H__
#define __CUBE_H__

#include "rubiksmap.h"
#include <stdio.h>

RubiksMap * cube_user_input();
RubiksMap * cube_identity();
int cube_faces_solved(RubiksMap * map);
int cube_is_solved(RubiksMap * map);
void cube_print_map(RubiksMap * map);

#endif
