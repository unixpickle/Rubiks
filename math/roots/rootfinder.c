#include "maproot.h"
#include "util/cmdutil.h"
#include <stdio.h>

void printUsage(const char * argv0);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        exit(1);
    }
    RubiksMap * premoves = NULL;
    int power = atoi(argv[1]);
    if (power < 2) {
        fprintf(stderr, "Invalid power: %s\n", argv[1]);
        exit(1);
    }
    int max = -1, i;
    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            exit(0);
        }
        if (strncmp(argv[i], "--max=", 6) == 0) {
            max = atoi(&argv[i][6]);
            continue;
        }
        RubiksMap * temp = parse_premoves_argument(argv[i]);
        if (temp) {
            premoves = temp;
            continue;
        }
        fprintf(stderr, "Unknown argument: %s\n", argv[i]);
        exit(1);
    }
    RubiksMap * map = cube_validated_user_input(premoves);
    if (premoves) rubiks_map_free(premoves);
    if (!map) return 0;
    
    int count;
    RubiksMap ** roots = maproot_take_nth_root(map, power, &count, max);
    printf("Found %d root%s!\n", count, count == 1 ? "" : "s");
    for (i = 0; i < count; i++) {
        StickerMap * theMap = rubiks_map_to_sticker_map(roots[i]);
        const char * str = sticker_map_to_user_string(theMap);
        printf("%s\n\n", str);
        sticker_map_free(theMap);
    }
    if (roots) free(roots);
    rubiks_map_free(map);
    return 0;
}

void printUsage(const char * argv0) {
    fprintf(stderr, "Usage: %s <power> [--help] [--premoves=ALG] [--max=n]\n", argv0);
}
