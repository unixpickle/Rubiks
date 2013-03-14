#include "cube.h"

void findSolutions(int length, RubiksMap ** operations, RubiksMap * input);

int main (int argc, const char * argv[]) {
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
	cube_print_map(input);
	findSolutions(1, operations, input);
	findSolutions(2, operations, input);
	findSolutions(3, operations, input);
	findSolutions(4, operations, input);
	findSolutions(5, operations, input);
finishMain:
	rubiks_map_free(input);
	for (i = 0; i < 12; i++) {
		rubiks_map_free(operations[i]);
	}
	free(operations);
	return 0;
}

void findSolutions(int length, RubiksMap ** operations, RubiksMap * input) {
	printf("Searching for %d-move solutions\n", length);
	int * indices = (int *)malloc(sizeof(int) * length);
	bzero(indices, sizeof(int) * length);
	int i, done = 0;
	RubiksMap * result = rubiks_map_new_identity();
	RubiksMap * tmp = rubiks_map_new_identity();
	while (!done) {
		rubiks_map_copy_into(result, input);
		for (i = 0; i < length; i++) {
			rubiks_map_copy_into(tmp, result);
			rubiks_map_multiply(result, operations[indices[i]], tmp);
		}
		if (cube_is_solved(result)) {
			printf("solved in %d moves: ", length);
			for (i = 0; i < length; i++) {
				printf("%d ", indices[i]);
			}
			printf("\n");
			goto functionFinish;
		}
		for (i = 0; i < length; i++) {
			indices[i]++;
			if (indices[i] >= 12) {
				if (i == length - 1) {
					done = 1;
				} else {
					indices[i] = 0;
				}
			} else {
				break;
			}
		}
	}
functionFinish:
	free(indices);
	rubiks_map_free(result);
	rubiks_map_free(tmp);
}
