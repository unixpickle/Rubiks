#ifndef __RUBIKS_MAP_H__
#define __RUBIKS_MAP_H__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stickermap.h"
#include "operations.h"

typedef struct {
    unsigned char pieces[20];
} RubiksMap;

RubiksMap * rubiks_map_user_input();
RubiksMap * rubiks_map_new_identity();
RubiksMap * rubiks_map_inverse(RubiksMap * map);
void rubiks_map_copy_into(RubiksMap * out, const RubiksMap * input);
void rubiks_map_multiply(RubiksMap * out, RubiksMap * left, RubiksMap * right);
void rubiks_map_operate(RubiksMap * out, RubiksMap * map, RubiksOperation o);
int rubiks_map_is_identity(RubiksMap * map);
void rubiks_map_free(RubiksMap * map);

RubiksMap * rubiks_map_from_sticker_map(StickerMap * stickers);
StickerMap * rubiks_map_to_sticker_map(RubiksMap * map);

#endif
