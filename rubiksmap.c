#include "rubiksmap.h"

RubiksMap * rubiks_map_new_identity() {
	RubiksMap * map = (RubiksMap *)malloc(sizeof(RubiksMap));
	bzero(map->indices, 54);
	int i;
	for (i = 0; i < 54; i++) {
		map->indices[i] = i;
	}
	return map;
}

RubiksMap * rubiks_map_copy(RubiksMap * map) {
	RubiksMap * mapCopy = (RubiksMap *)malloc(sizeof(RubiksMap));
	memcpy(mapCopy->indices, map->indices, 54 * sizeof(unsigned char));
	return mapCopy;
}

RubiksMap * rubiks_map_copy_into(RubiksMap * dest, RubiksMap * map) {
	int i;
	memcpy(dest->indices, map->indices, 54 * sizeof(unsigned char));
}

void rubiks_map_rotate(RubiksMap * map,
					   const unsigned char * indices,
					   int indexCount,
					   int shiftFactor) {
	unsigned char * mapValues = (unsigned char *)malloc(indexCount);
	int i;
	for (i = 0; i < indexCount; i++) {
		mapValues[i] = map->indices[indices[i]];
	}
	for (i = 0; i < indexCount; i++) {
		int index = i - shiftFactor;
		while (index < 0) {
			index += indexCount;
		}
		index = index % indexCount; // for negative shifting
		map->indices[indices[i]] = mapValues[index];
	}
	free(mapValues);
}

void rubiks_map_multiply(RubiksMap * output,
						 const RubiksMap * left,
						 const RubiksMap * right) {
	int i;
	for (i = 0; i < 54; i++) {
		output->indices[i] = right->indices[left->indices[i]];
	}
}

RubiksMap * rubiks_map_inverse(RubiksMap * map) {
	RubiksMap * mapInv = rubiks_map_new_identity();
	int i;
	for (i = 0; i < 54; i++) {
		unsigned char mapIndex = map->indices[i];
		mapInv->indices[mapIndex] = i;
	}
	return mapInv;
}

void rubiks_map_free(RubiksMap * map) {
	free(map);
}

RubiksMap * rubiks_map_create_top() {
	unsigned char topIndices[] = {29, 28, 27, 15, 3, 4, 5, 17};
	unsigned char topRing[] = {26, 14, 2, 38, 44, 50, 6, 18, 30, 51, 45, 39};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, topIndices, 8, 2);
	rubiks_map_rotate(ident, topRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_bottom() {
	unsigned char bottomIndices[] = {33, 34, 35, 23, 11, 10, 9, 21};
	unsigned char bottomRing[] = {24, 12, 0, 36, 42, 48, 8, 20, 32, 53, 47, 41};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, bottomIndices, 8, 2);
	rubiks_map_rotate(ident, bottomRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_right() {
	unsigned char rightIndices[] = {39, 40, 41, 47, 53, 52, 51, 45};
	unsigned char rightRing[] = {26, 25, 24, 35, 34, 33, 32, 31, 30, 29, 28, 27};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, rightIndices, 8, 2);
	rubiks_map_rotate(ident, rightRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_left() {
	unsigned char leftIndices[] = {38, 37, 36, 42, 48, 49, 50, 44};
	unsigned char leftRing[] = {2, 1, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, leftIndices, 8, 2);
	rubiks_map_rotate(ident, leftRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_front() {
	unsigned char frontIndices[] = {26, 25, 24, 12, 0, 1, 2, 14};
	unsigned char frontRing[] = {3, 15, 27, 39, 40, 41, 35, 23, 11, 36, 37, 38};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, frontIndices, 8, 2);
	rubiks_map_rotate(ident, frontRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_back() {
	unsigned char backIndices[] = {30, 31, 32, 20, 8, 7, 6, 18};
	unsigned char backRing[] = {5, 17, 29, 51, 52, 53, 33, 21, 9, 48, 49, 50};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, backIndices, 8, 2);
	rubiks_map_rotate(ident, backRing, 12, 3);
	return ident;
}
