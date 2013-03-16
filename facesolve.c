#include "cubesearch.h"

int cube_has_face_solved(RubiksMap * map);

int main(int argc, const char * argv[]) {
	RubiksMap ** operations = (RubiksMap **)malloc(sizeof(RubiksMap *) * 12);
	operations[0] = rubiks_map_create_top();
	operations[1] = rubiks_map_create_bottom();
	operations[2] = rubiks_map_create_right();
	operations[3] = rubiks_map_create_left();
	operations[4] = rubiks_map_create_front();
	operations[5] = rubiks_map_create_back();
	int i;
	for (i = 0; i < 6; i++) {
		operations[i + 6] = rubiks_map_inverse(operations[i]);
	}

	printf("Brute force rubix cube solver\n");
	RubiksMap * input = cube_user_input();
	if (!input) {
		fprintf(stderr, "Invalid input\n");
		goto finishMain;
	}
	if (cube_is_solved(input)) {
		printf("you input a solved cube!\n");
		goto finishMain;
	}
	printf("Internal cube data: ");
	cube_print_map(input);
	printf("Operations:\n\
0 = top        6 =  inverse top\n\
1 = bottom     7 =  inverse bottom\n\
2 = right      8 =  inverse right\n\
3 = left       9 =  inverse left\n\
4 = front      10 = inverse front\n\
5 = back       11 = inverse back\n\n\n");
	for (i = 1; i < 21; i++) {
		findSolutions(i, operations, input, cube_has_face_solved);
	}
finishMain:
	rubiks_map_free(input);
	for (i = 0; i < 12; i++) {
		rubiks_map_free(operations[i]);
	}
	free(operations);
	return 0;
}

int cube_has_face_solved(RubiksMap * map) {
	if (cube_faces_solved(map) >= 1) return 1;
	return 0;
}
