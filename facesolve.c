#include "cubesearch.h"

int cube_has_face_solved(RubiksMap * map);

int main(int argc, const char * argv[]) {
    RubiksMap ** operations = cube_standard_face_turns();
    int i;
    printf("Brute force rubix cube solver\n");
    RubiksMap * input = rubiks_map_user_input();
    if (!input) {
        fprintf(stderr, "Invalid input\n");
        goto finishMain;
    }
    if (rubiks_map_is_identity(input)) {
        printf("you input a solved cube!\n");
        goto finishMain;
    }
    cube_print_solution_key();
    for (i = 1; i < 21; i++) {
        findSolutions(i, operations, input, cube_has_face_solved);
    }
finishMain:
    rubiks_map_free(input);
    for (i = 0; i < 18; i++) {
        rubiks_map_free(operations[i]);
    }
    free(operations);
    return 0;
}

int cube_has_face_solved(RubiksMap * map) {
    StickerMap * stickers = rubiks_map_to_sticker_map(map);
    int status = 0;
    status = sticker_map_faces_solved(stickers) > 0;
    sticker_map_free(stickers);
    return status;
}
