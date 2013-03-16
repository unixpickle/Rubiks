#ifndef __RUBIKS_MAP_H__
#define __RUBIKS_MAP_H__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
	unsigned char indices[54];
	unsigned char padding[2];
} RubiksMap;

RubiksMap * rubiks_map_new_identity();
RubiksMap * rubiks_map_copy(RubiksMap * map);
void rubiks_map_rotate(RubiksMap * map,
					   const unsigned char * indices,
					   int indexCount,
					   int shiftFactor);
void rubiks_map_multiply(RubiksMap * output,
						 const RubiksMap * left,
						 const RubiksMap * right);
RubiksMap * rubiks_map_inverse(RubiksMap * map);
void rubiks_map_free(RubiksMap * map);

RubiksMap * rubiks_map_create_top();
RubiksMap * rubiks_map_create_bottom();
RubiksMap * rubiks_map_create_right();
RubiksMap * rubiks_map_create_left();
RubiksMap * rubiks_map_create_front();
RubiksMap * rubiks_map_create_back();

#endif
