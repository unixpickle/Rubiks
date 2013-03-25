#ifndef __STICKER_MAP_H__
#define __STICKER_MAP_H__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
	unsigned char indices[54];
	unsigned char padding[2];
} StickerMap;

StickerMap * sticker_map_new_identity();
StickerMap * sticker_map_copy(StickerMap * map);
void sticker_map_rotate(StickerMap * map,
					   const unsigned char * indices,
					   int indexCount,
					   int shiftFactor);
void sticker_map_multiply(StickerMap * output,
						 const StickerMap * left,
						 const StickerMap * right);
StickerMap * sticker_map_inverse(StickerMap * map);
void sticker_map_free(StickerMap * map);

StickerMap * sticker_map_create_top();
StickerMap * sticker_map_create_bottom();
StickerMap * sticker_map_create_right();
StickerMap * sticker_map_create_left();
StickerMap * sticker_map_create_front();
StickerMap * sticker_map_create_back();

StickerMap * sticker_map_user_input();
StickerMap * sticker_map_identity();
int sticker_map_faces_solved(StickerMap * map);
int sticker_map_is_solved(StickerMap * map);
void sticker_map_print(StickerMap * map);

#endif
