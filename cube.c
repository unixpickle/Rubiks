#include "cube.h"

static int _map_apply_face(RubiksMap * map, const int * indices, const char * str);
const static int frontIndices[] = {3, 15, 27, 2, 14, 26, 1, 13, 25};
const static int backIndices[] = {31, 19, 7, 32, 20, 8, 9, 21, 33};
const static int rightIndices[] = {40, 46, 52, 41, 47, 53, 42, 48, 54};
const static int leftIndices[] = {51, 45, 39, 50, 44, 38, 49, 43, 37};
const static int topIndices[] = {6, 18, 30, 5, 17, 29, 4, 16, 28};
const static int bottomIndices[] = {12, 24, 36, 11, 23, 35, 10, 22, 34};

RubiksMap * cube_user_input() {
	RubiksMap * map = rubiks_map_new_identity();
	char inputString[64];
	printf("Colors:\
  white:   1\
  yellow:  2\
  blue:    3\
  green:   4\
  red:     5\
  orange:  6\n\n");
	printf("** Enter colors from left-to-right top-to-bottom.**\n");
	printf("Enter first side: ");
	fflush(stdout);
	fgets(inputString, 64, stdin);
	if (!_map_apply_face(map, frontIndices, inputString)) goto failureHandler;
	printf("Enter back side (turn around vertical axis): ");
	fflush(stdout);
	fgets(inputString, 64, stdin);
	if (!_map_apply_face(map, backIndices, inputString)) goto failureHandler;
	printf("Enter top side: ");
	fflush(stdout);
	fgets(inputString, 64, stdin);
	if (!_map_apply_face(map, topIndices, inputString)) goto failureHandler;
	printf("Enter bottom side: ");
	fflush(stdout);
	fgets(inputString, 64, stdin);
	if (!_map_apply_face(map, bottomIndices, inputString)) goto failureHandler;
	printf("Enter right side: ");
	fflush(stdout);
	fgets(inputString, 64, stdin);
	if (!_map_apply_face(map, rightIndices, inputString)) goto failureHandler;
	printf("Enter left side: ");
	fflush(stdout);
	fgets(inputString, 64, stdin);
	if (!_map_apply_face(map, leftIndices, inputString)) goto failureHandler;
	return map;

failureHandler:
	rubiks_map_free(map);
	return NULL;
}

RubiksMap * cube_identity() {
	RubiksMap * map = rubiks_map_new_identity();
	_map_apply_face(map, topIndices, "333333333");
	_map_apply_face(map, bottomIndices, "444444444");
	_map_apply_face(map, leftIndices, "666666666");
	_map_apply_face(map, rightIndices, "555555555");
	_map_apply_face(map, frontIndices, "111111111");
	_map_apply_face(map, backIndices, "222222222");
	return map;
}

static int _map_apply_face(RubiksMap * map, const int * indices, const char * str) {
	int i;
	for (i = 0; i < 9; i++) {
		char c = str[i];
		if (c < '1' || c > '6') return 0;
		unsigned char value = (unsigned char)c - '1' + 1;
		map->indices[indices[i]] = value;
	}
	return 1;
}
