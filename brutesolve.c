#include "cubesearch.h"

int main (int argc, const char * argv[]) {
    RubiksMap ** operations = cube_standard_face_turns();
    int i;
    
    printf("Brute force rubix cube solver\n");
    RubiksMap * input = rubiks_map_user_input();
    printf("\nInternal representation: ");
    for (i = 0; i < 20; i++) {
        printf("%d ", input->pieces[i]);
    }
    printf("\n\n");
    if (!input) {
        fprintf(stderr, "Invalid input\n");
        goto finishMain;
    }
    if (rubiks_map_is_identity(input)) {
        printf("you input a solved cube!\n");
        goto finishMain;
    }
    cube_print_solution_key();
    printf("\n\n");
    for (i = 1; i < 21; i++) {
        findSolutions(i, operations, input, rubiks_map_is_identity);
    }
finishMain:
    rubiks_map_free(input);
    for (i = 0; i < 18; i++) {
        rubiks_map_free(operations[i]);
    }
    free(operations);
    return 0;
}
