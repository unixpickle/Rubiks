#include "rubiksmap.h"

RubiksMap * rubiks_map_new_identity() {
	RubiksMap * map = (RubiksMap *)malloc(sizeof(RubiksMap));
	bzero(map->indices, 54);
	int i;
	for (i = 0; i < 54; i++) {
		map->indices[i] = i;
	}
}

RubiksMap * rubiks_map_copy(RubiksMap * map) {
	RubiksMap * mapCopy = (RubiksMap *)malloc(sizeof(RubiksMap));
	memcpy(mapCopy->indices, map->indices, 54);
	return mapCopy;
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
		map->indices[i] = mapValues[index];
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

void rubiks_map_free(RubiksMap * map) {
	free(map);
}

RubiksMap * rubiks_map_create_top() {
	unsigned char topIndices[] = {30, 29, 28, 16, 4, 5, 6, 18};
	unsigned char topRing[] = {27, 15, 3, 39, 45, 51, 7, 19, 31, 52, 46, 40};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, topIndices, 8, 2);
	rubiks_map_rotate(ident, topRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_bottom() {
	unsigned char bottomIndices[] = {34, 35, 36, 24, 12, 11, 10, 22};
	unsigned char bottomRing[] = {25, 13, 1, 37, 43, 49, 9, 21, 33, 54, 48, 42};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, bottomIndices, 8, 2);
	rubiks_map_rotate(ident, bottomRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_right() {
	unsigned char rightIndices[] = {40, 41, 42, 48, 54, 53, 52, 46};
	unsigned char rightRing[] = {27, 26, 25, 36, 35, 34, 33, 32, 31, 30, 29, 28};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, rightIndices, 8, 2);
	rubiks_map_rotate(ident, rightRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_left() {
	unsigned char leftIndices[] = {39, 38, 37, 43, 49, 50, 51, 45};
	unsigned char leftRing[] = {3, 2, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, leftIndices, 8, 2);
	rubiks_map_rotate(ident, leftRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_front() {
	unsigned char frontIndices[] = {27, 26, 25, 13, 1, 2, 3, 15};
	unsigned char frontRing[] = {4, 16, 28, 40, 41, 42, 36, 24, 12, 37, 38, 39};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, frontIndices, 8, 2);
	rubiks_map_rotate(ident, frontRing, 12, 3);
	return ident;
}

RubiksMap * rubiks_map_create_back() {
	unsigned char backIndices[] = {31, 32, 33, 21, 9, 8, 7, 19};
	unsigned char backRing[] = {6, 18, 30, 52, 53, 54, 34, 22, 10, 49, 50, 51};
	RubiksMap * ident = rubiks_map_new_identity();
	rubiks_map_rotate(ident, backIndices, 8, 2);
	rubiks_map_rotate(ident, backRing, 12, 3);
	return ident;
}
