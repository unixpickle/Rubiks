#include "StickerMap.h"

StickerMap * sticker_map_new_identity() {
	StickerMap * map = (StickerMap *)malloc(sizeof(StickerMap));
	bzero(map->indices, 54);
	int i;
	for (i = 0; i < 54; i++) {
		map->indices[i] = i;
	}
	return map;
}

StickerMap * sticker_map_copy(StickerMap * map) {
	StickerMap * mapCopy = (StickerMap *)malloc(sizeof(StickerMap));
	memcpy(mapCopy->indices, map->indices, 54 * sizeof(unsigned char));
	return mapCopy;
}

StickerMap * sticker_map_copy_into(StickerMap * dest, StickerMap * map) {
	int i;
	memcpy(dest->indices, map->indices, 54 * sizeof(unsigned char));
}

void sticker_map_rotate(StickerMap * map,
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

void sticker_map_multiply(StickerMap * output,
						 const StickerMap * left,
						 const StickerMap * right) {
	int i;
	for (i = 0; i < 54; i++) {
		output->indices[i] = right->indices[left->indices[i]];
	}
}

StickerMap * sticker_map_inverse(StickerMap * map) {
	StickerMap * mapInv = sticker_map_new_identity();
	int i;
	for (i = 0; i < 54; i++) {
		unsigned char mapIndex = map->indices[i];
		mapInv->indices[mapIndex] = i;
	}
	return mapInv;
}

void sticker_map_free(StickerMap * map) {
	free(map);
}

StickerMap * sticker_map_create_top() {
	unsigned char topIndices[] = {29, 28, 27, 15, 3, 4, 5, 17};
	unsigned char topRing[] = {26, 14, 2, 38, 44, 50, 6, 18, 30, 51, 45, 39};
	StickerMap * ident = sticker_map_new_identity();
	sticker_map_rotate(ident, topIndices, 8, 2);
	sticker_map_rotate(ident, topRing, 12, 3);
	return ident;
}

StickerMap * sticker_map_create_bottom() {
	unsigned char bottomIndices[] = {33, 34, 35, 23, 11, 10, 9, 21};
	unsigned char bottomRing[] = {24, 12, 0, 36, 42, 48, 8, 20, 32, 53, 47, 41};
	StickerMap * ident = sticker_map_new_identity();
	sticker_map_rotate(ident, bottomIndices, 8, 2);
	sticker_map_rotate(ident, bottomRing, 12, 3);
	return ident;
}

StickerMap * sticker_map_create_right() {
	unsigned char rightIndices[] = {39, 40, 41, 47, 53, 52, 51, 45};
	unsigned char rightRing[] = {26, 25, 24, 35, 34, 33, 32, 31, 30, 29, 28, 27};
	StickerMap * ident = sticker_map_new_identity();
	sticker_map_rotate(ident, rightIndices, 8, 2);
	sticker_map_rotate(ident, rightRing, 12, 3);
	return ident;
}

StickerMap * sticker_map_create_left() {
	unsigned char leftIndices[] = {38, 37, 36, 42, 48, 49, 50, 44};
	unsigned char leftRing[] = {2, 1, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3};
	StickerMap * ident = sticker_map_new_identity();
	sticker_map_rotate(ident, leftIndices, 8, 2);
	sticker_map_rotate(ident, leftRing, 12, 3);
	return ident;
}

StickerMap * sticker_map_create_front() {
	unsigned char frontIndices[] = {26, 25, 24, 12, 0, 1, 2, 14};
	unsigned char frontRing[] = {3, 15, 27, 39, 40, 41, 35, 23, 11, 36, 37, 38};
	StickerMap * ident = sticker_map_new_identity();
	sticker_map_rotate(ident, frontIndices, 8, 2);
	sticker_map_rotate(ident, frontRing, 12, 3);
	return ident;
}

StickerMap * sticker_map_create_back() {
	unsigned char backIndices[] = {30, 31, 32, 20, 8, 7, 6, 18};
	unsigned char backRing[] = {5, 17, 29, 51, 52, 53, 33, 21, 9, 48, 49, 50};
	StickerMap * ident = sticker_map_new_identity();
	sticker_map_rotate(ident, backIndices, 8, 2);
	sticker_map_rotate(ident, backRing, 12, 3);
	return ident;
}
