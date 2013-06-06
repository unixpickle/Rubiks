#include <stdio.h>
#include "randcube.h"

int main(int argc, const char * argv[]) {
    
    RubiksMap * map = rand_rubiks_map();
    StickerMap * stickers = rubiks_map_to_sticker_map(map);
    const char * str = sticker_map_to_user_string(stickers);
    printf("Here is your scramble:\n\n%s\n\n", str);
    sticker_map_free(stickers);
    rubiks_map_free(map);
    
    return 0;
}
