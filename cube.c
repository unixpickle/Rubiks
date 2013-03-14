#include "cube.h"

static int _map_apply_face(RubiksMap * map, const int * indices, const char * str);
const static int frontIndices[] = {2, 14, 26, 1, 13, 25, 0, 12, 24};
const static int backIndices[] = {30, 18, 6, 31, 19, 7, 32, 20, 8};
const static int rightIndices[] = {39, 45, 51, 40, 46, 52, 41, 47, 53};
const static int leftIndices[] = {50, 44, 38, 49, 43, 37, 48, 42, 36};
const static int topIndices[] = {5, 17, 29, 4, 16, 28, 3, 15, 27};
const static int bottomIndices[] = {11, 23, 35, 10, 22, 34, 9, 21, 33};

RubiksMap * cube_user_input() {
	RubiksMap * map = rubiks_map_new_identity();
	char inputString[64];
	printf("Colors:\n\
  white:   1\n\
  yellow:  2\n\
  blue:    3\n\
  green:   4\n\
  red:     5\n\
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

int cube_is_solved(RubiksMap * map) {
	char faceFront = map->indices[frontIndices[0]];
	char faceBack = map->indices[backIndices[0]];
	char faceRight = map->indices[rightIndices[0]];
	char faceLeft = map->indices[leftIndices[0]];
	char faceTop = map->indices[topIndices[0]];
	char faceBottom = map->indices[bottomIndices[0]];
	int i;
	for (i = 1; i < 9; i++) {
		if (map->indices[frontIndices[i]] != faceFront) return 0;
		if (map->indices[backIndices[i]] != faceBack) return 0;
		if (map->indices[rightIndices[i]] != faceRight) return 0;
		if (map->indices[leftIndices[i]] != faceLeft) return 0;
		if (map->indices[topIndices[i]] != faceTop) return 0;
		if (map->indices[bottomIndices[i]] != faceBottom) return 0;
	}
	printf("cube is solved.\n");
	return 1;
}

void cube_print_map(RubiksMap * map) {
	int i;
	for (i = 0; i < 54; i++) {
		printf("%d%s", map->indices[i], i == 53 ? "" : " ");
	}
	printf("\n");
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
