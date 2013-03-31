#include "pocketmap.h"

const static unsigned char IdentityData[] = {0, 1, 2, 3, 4, 5, 6, 7};
const static struct {
    int cornerIndices[4];
    int cornerDestinations[4];
    int symmetry;
} _SmallOperationTable[] = {
    // top
    {{0x2, 0x3, 0x7, 0x6},
        {0x6, 0x2, 0x3, 0x7},
        3},
    // bottom
    {{0x0, 0x1, 0x5, 0x4},
        {0x4, 0x0, 0x1, 0x5},
        3},
    // right
    {{0x7, 0x6, 0x4, 0x5},
        {0x5, 0x7, 0x6, 0x4},
        2},
    // left
    {{0x3, 0x2, 0x0, 0x1},
        {0x1, 0x3, 0x2, 0x0},
        2},
    // front
    {{0x3, 0x7, 0x5, 0x1},
        {0x7, 0x5, 0x1, 0x3},
        1},
    // back
    {{0x2, 0x6, 0x4, 0x0},
        {0x6, 0x4, 0x0, 0x2},
        1}
};

static int _read_cube_face(unsigned char * output);
static PocketMap * _process_pocket_map(const unsigned char * data);

PocketMap * pocket_map_user_input() {
    char * faces[6] = {"Front  [WHITE]: ",
                       "Back  [YELLOW]: ",
                       "Top     [BLUE]: ",
                       "Bottom [GREEN]: ",
                       "Right    [RED]: ",
                       "Left  [ORANGE]: "};
    unsigned char cubeData[24];
    int i;
    printf("Enter each face truthfully:\n");
    for (i = 0; i < 6; i++) {
        printf("%s", faces[i]);
        fflush(stdout);
        if (!_read_cube_face(&cubeData[i * 4])) return NULL;
    }
    // convert the cubeData to a PocketMap
    return _process_pocket_map(cubeData);
}

PocketMap * pocket_map_new_identity() {
    PocketMap * map = (PocketMap *)malloc(sizeof(PocketMap));
    memcpy(map->pieces, IdentityData, 8);
    return map;
}

PocketMap * pocket_map_inverse(PocketMap * map) {
    PocketMap * inverse = (PocketMap *)malloc(sizeof(PocketMap));
    int i;
    for (i = 0; i < 8; i++) {
        unsigned char piece = map->pieces[i];
        int sourceIndex = piece & 7;
        unsigned char inv = symmetry_operation_inverse((piece >> 4) & 7);
        inverse->pieces[sourceIndex] = i | (inv << 4);
    }
    return inverse;
}

void pocket_map_multiply(PocketMap * out, PocketMap * left, PocketMap * right) {
    int i;
    for (i = 0; i < 8; i++) {
        unsigned char leftPiece = left->pieces[i];
		if (leftPiece == i) { // it is an identity piece
			out->pieces[i] = right->pieces[i];
			continue;
		}
        int fromIndex = leftPiece & 7;
        unsigned char piece = right->pieces[fromIndex];
        int orientation = (piece >> 4) & 7;
        int leftOrientation = (leftPiece >> 4) & 7;
        orientation = symmetry_operation_compose(leftOrientation, orientation);
        out->pieces[i] = (piece & 7) | (orientation << 4);
    }
}

void pocket_map_operate(PocketMap * out, PocketMap * map, RubiksOperation o) {
    int i;
    memcpy(out->pieces, map->pieces, 8);
    int symmetry = _SmallOperationTable[o].symmetry;
    for (i = 0; i < 4; i++) {
        int sourceIndex = _SmallOperationTable[o].cornerIndices[i];
        int destIndex = _SmallOperationTable[o].cornerDestinations[i];
        unsigned char piece = map->pieces[sourceIndex];
        int newSym = symmetry_operation_compose(symmetry, (piece >> 4) & 7);
        out->pieces[destIndex] = (piece & 7) | (newSym << 4);
    }
}

int pocket_map_is_identity(PocketMap * map) {
    return (memcmp(map->pieces, IdentityData, 8) == 0);
}

void pocket_map_free(PocketMap * map) {
    free(map);
}

/////////////
// PRIVATE //
/////////////

static int _read_cube_face(unsigned char * output) {
    char str[512];
    int i, len;
    fgets(str, 512, stdin);
    len = strlen(str);
    if (len == 0) return 0;
    if (str[len - 1] == '\n') str[len - 1] = 0;
    if (len != 5) return 0;
    for (i = 0; i < 4; i++) {
        if (str[i] < '1' || str[i] > '6') return 0;
        output[i] = str[i] - '0';
    }
    return 1;
}

static PocketMap * _process_pocket_map(const unsigned char * data) {
    const unsigned char RealCornerIndices[12][3] = {
        {22, 14, 7},
        {23, 12, 2},
        {20, 8, 5},
        {21, 10, 0},
        {19, 15, 6},
        {18, 13, 3},
        {17, 9, 4},
        {16, 11, 1}
    };
    PocketMap * map = (PocketMap *)malloc(sizeof(PocketMap));
    int i, j;
    // place each of the corners
    for (i = 0; i < 8; i++) {
        unsigned char realColors[3];
        for (j = 0; j < 3; j++) {
            realColors[j] = data[RealCornerIndices[i][j]];
        }
        int realIndex = -1, symmetry = -1;
        for (j = 0; j < 8; j++) {
            symmetry = symmetry_operation_find(CornerPieces[j], realColors);
            if (symmetry >= 0) {
                realIndex = j;
                break;
            }
        }
        if (realIndex < 0) {
            printf("Corner piece not found: ");
            for (j = 0; j < 3; j++) {
                printf("%d ", realColors[j]);
            }
            printf("\n");
            pocket_map_free(map);
            return NULL;
        }
        map->pieces[i] = realIndex | (symmetry << 4);
    }
    return map;
}
