#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "representation/operations.h"
#include "representation/stickerindices.h"
#include "representation/symmetry.h"

typedef struct {
    char pieces[8];
} PocketMap;

PocketMap * pocket_map_user_input();
PocketMap * pocket_map_new_identity();
PocketMap * pocket_map_inverse(PocketMap * map);
void pocket_map_multiply(PocketMap * out, PocketMap * left, PocketMap * right);
void pocket_map_operate(PocketMap * out, PocketMap * map, RubiksOperation o);
int pocket_map_is_identity(PocketMap * map);
void pocket_map_free(PocketMap * map);
