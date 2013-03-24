#ifndef __RUBIKS_MAP_H__
#define __RUBIKS_MAP_H__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stickermap.h"

typedef struct {
    unsigned char pieces[20];
} RubiksMap;

typedef enum {
    RubiksOperationTop = 0,
    RubiksOperationBottom = 1,
    RubiksOperationRight = 2,
    RubiksOperationLeft = 3,
    RubiksOperationFront = 4,
    RubiksOperationBack = 5
} RubiksOperation;

RubiksMap * rubiks_map_new_identity();
RubiksMap * rubiks_map_inverse(RubiksMap * map);
void rubiks_map_multiply(RubiksMap * out, RubiksMap * left, RubiksMap * right);
void rubiks_map_operate(RubiksMap * out, RubiksMap * map, RubiksOperation o);
int rubiks_map_is_identity(RubiksMap * map);
void rubiks_map_free(RubiksMap * map);

RubiksMap * rubiks_map_from_sticker_map(StickerMap * stickers);

#endif