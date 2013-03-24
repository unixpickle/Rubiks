#include "rubiksmap.h"

static int _symmetry_operation_compose(int left, int right);
static int _symmetry_operation_inverse(int op);
static int _symmetry_operation_find(unsigned char * orig, unsigned char * res);
static void _symmetry_operation_perform(int op, unsigned char * data);
static const int _IdentityMapData[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

// optimized rubiks operation methods
const static struct {
    int cornerIndices[4];
    int edgeIndices[4];
    int cornerDestinations[4];
    int edgeDestinations[4];
    int symmetry;
} _OperationTable[] = {
    // top
    {{0b010, 0b011, 0b111, 0b110},
        {0, 4, 6, 5},
        {0b110, 0b010, 0b011, 0b111},
        {4, 6, 5, 0},
        3},
    // bottom
    {{0b000, 0b001, 0b101, 0b100},
        {2, 10, 8, 11},
        {0b100, 0b000, 0b001, 0b101},
        {10, 8, 11, 2},
        3},
    // right
    {{0b111, 0b110, 0b100, 0b101},
        {11, 1, 5, 7},
        {0b101, 0b111, 0b110, 0b100},
        {7, 11, 1, 5},
        2},
    // left
    {{0b011, 0b010, 0b000, 0b001},
        {10, 3, 4, 9},
        {0b001, 0b011, 0b010, 0b000},
        {9, 10, 3, 4},
        2},
    // front
    {{0b011, 0b111, 0b101, 0b001},
        {0, 1, 2, 3},
        {0b111, 0b101, 0b001, 0b011},
        {1, 2, 3, 0},
        1},
    // back
    {{0b010, 0b110, 0b100, 0b000},
        {6, 7, 8, 9},
        {0b110, 0b100, 0b000, 0b010},
        {7, 8, 9, 6},
        1}
};

RubiksMap * rubiks_map_new_identity() {
	RubiksMap * map = (RubiksMap *)malloc(sizeof(RubiksMap));
    memcpy(map->pieces, _IdentityMapData, 20);
	return map;
}

RubiksMap * rubiks_map_inverse(RubiksMap * map) {
    RubiksMap * inverse = (RubiksMap *)malloc(sizeof(RubiksMap));
	int i;
    for (i = 0; i < 8; i++) {
        unsigned char piece = map->pieces[i];
        int sourceIndex = piece & 7;
        unsigned char inv = _symmetry_operation_inverse((piece >> 4) & 7);
        inverse->pieces[sourceIndex] = i | (inv << 4);
    }
    for (i = 0; i < 12; i++) {
        unsigned char piece = map->pieces[i + 8];
        int sourceIndex = piece & 15;
        unsigned char inv = _symmetry_operation_inverse((piece >> 4) & 7);
        inverse->pieces[sourceIndex + 8] = i | (inv << 4);
    }
    return inverse;
}

void rubiks_map_multiply(RubiksMap * out, RubiksMap * left, RubiksMap * right) {
	// NOTE: the left map describes its operation on the identity cube.

	int i;
	// multiply the corners first
	for (i = 0; i < 8; i++) {
		unsigned char leftPiece = left->pieces[i];
		int fromIndex = leftPiece & 7;
		unsigned char piece = right->pieces[fromIndex];
		int orientation = (piece >> 4) & 7;
		int leftOrientation = (leftPiece >> 4) & 7;
		orientation = _symmetry_operation_compose(leftOrientation, orientation);
		out->pieces[i] = (piece & 7) | (orientation << 4);
	}
    // multiply the edges
    for (i = 0; i < 12; i++) {
        unsigned char leftPiece = left->pieces[i + 8];
        int fromIndex = leftPiece & 15;
        unsigned char piece = right->pieces[fromIndex + 8];
        int orientation = (piece >> 4) & 7;
        int leftOrientation = (leftPiece >> 4) & 7;
        orientation = _symmetry_operation_compose(leftOrientation, orientation);
        out->pieces[i + 8] = (piece & 7) | (orientation << 4);
    }
}

void rubiks_map_operate(RubiksMap * out, RubiksMap * map, RubiksOperation o) {
    memcpy(out->pieces, map->pieces, 20);
    int i;
    int symmetry = _OperationTable[o].symmetry;
    for (i = 0; i < 4; i++) {
        int sourceIndex = _OperationTable[o].cornerIndices[i];
        int destIndex = _OperationTable[o].cornerDestinations[i];
        unsigned char piece = map->pieces[sourceIndex];
        int newSym = _symmetry_operation_compose(symmetry, (piece >> 4) & 7);
        out->pieces[destIndex] = (piece & 7) | (newSim << 4);
    }
    for (i = 0; i < 4; i++) {
        int sourceIndex = _OperationTable[o].edgeIndices[i];
        int destIndex = _OperationTable[o].edgeDestinations[i];
        unsigned char piece = map->pieces[sourceIndex + 8];
        int newSym = _symmetry_operation_compose(symmetry, (piece >> 4) & 7);
        out->pieces[destIndex + 8] = (piece & 15) | (newSim << 4);
    }
}

int rubiks_map_is_identity(RubiksMap * map) {
    return (memcpy(map->pieces, _IdentityMapData, 20) == 0);
}

void rubiks_map_free(RubiksMap * map) {
    free(map);
}

RubiksMap * rubiks_map_from_sticker_map(StickerMap * stickers) {
    // doing a task like this requires a LOT of patients...and organization
    const struct {
        int xColor;
        int yColor;
        int zColor;
    } CornerPieces[] = {
        {6,4,2}, // 000
        {6,4,1}, // 001
        {6,3,2}, // 010
        {6,3,1}, // 011
        {5,4,2}, // 100
        {5,4,1}, // 101
        {5,3,2}, // 110
        {5,3,1}  // 111
    };
    const struct {
        int xColor;
        int yColor;
        int zColor;
    } EdgePieces[] = {
        {0,3,1},
        {5,0,1},
        {0,4,1},
        {6,0,1},
        {6,3,0},
        {5,3,0},
        {0,3,2},
        {5,0,2},
        {0,4,2},
        {6,0,2},
        {6,4,0},
        {5,4,0}
    };
    const struct {
        int xIndex;
        int yIndex;
        int zIndex;
    } CornerIndices[] = {
        {48, 9, 8},   // 000
        {36, 11, 0},  // 001
        {50, 5, 6},   // 010
        {38, 3, 2},   // 011
        {53, 33, 32}, // 100
        41, 35, 24},  // 101
        {51, 29, 30}, // 110
        {39, 27, 26}, // 111
    };
    const struct {
        int xIndex;
        int yIndex;
        int zIndex;
    } EdgeIndices[] = {
        {-1, 15, 14},
        {40, -1, 25},
        {-1, 23, 12},
        {37, -1, 1},
        {44, 4, -1},
        {45, 28, -1},
        {-1, 17, 18},
        {52, -1, 31},
        {-1, 21, 20},
        {49, -1, 7},
        {42, 11, -1},
        {47, 34, -1}
    };

    RubiksMap * map = rubiks_map_new_identity();

    int i, j;
    // place each of the corners
    for (i = 0; i < 8; i++) {
        unsigned char realColors[3];
        for (j = 0; j < 3; j++) {
            realColors[j] = stickers->indices[CornerIndices[i][j]];
        }
        int realIndex = -1, symmetry = -1;
        for (j = 0; j < 8; j++) {
            symmetry = _symmetry_operation_find(CornerPieces[j], realColors);
            if (symmetry >= 0) {
                realIndex = j;
                break;
            }
        }
        if (realIndex < 0) {
            rubiks_map_free(map);
            return NULL;
        }
        map->pieces[i] = realIndex | (symmetry << 4);
    }
    // place all of the edge pieces
    for (i = 0; i < 12; i++) {
        unsigned char realColors[3];
        bzero(realColors, 3);
        for (j = 0; j < 3; j++) {
            int index = EdgeIndices[i][j];
            if (index < 0) continue;
            realColors[j] = index;
        }
        int realIndex = -1, symmetry = -1;
        for (j = 0; j < 12; j++) {
            symmetry = _symmetry_operation_find(EdgePieces[j], realColors);
            if (symmetry >= 0) {
                realIndex = j;
                break;
            }
        }
        if (realIndex < 0) {
            rubiks_map_free(map);
            return NULL;
        }
        map->pieces[i + 8] = realIndex | (symmetry << 4);
    }

    return map;
}

////////////////////////////
// PRIVATE STATIC METHODS //
////////////////////////////

static int _symmetry_operation_compose(int left, int right) {
	const int table[] = {0, 1, 2, 3, 4, 5,
						 1, 0, 4, 5, 2, 3,
						 2, 5, 0, 4, 3, 1,
						 3, 4, 5, 0, 1, 2,
						 4, 3, 1, 2, 5, 0,
						 5, 2, 3, 1, 0, 4};
	return table[6 * left + right];
}

static int _symmetry_operation_inverse(int op) {
    const int table[] = {0, 1, 2, 3, 5, 4};
    return table[op];
}

static int _symmetry_operation_find(unsigned char * orig, unsigned char * res) {
    if (memcmp(orig, res, 3) == 0) return 0;
    if (res[0] == orig[0]) return 2;
    if (res[2] == orig[2]) return 1;
    if (res[1] == orig[1]) return 3;
    if (res[0] == orig[2]) return 4;
    if (res[2] == orig[0]) return 5;
    return -1;
}

static void _symmetry_operation_perform(int op, unsigned char * data) {
	if (op == 0) return;
	unsigned char el1 = data[0];
	unsigned char el2 = data[1];
	unsigned char el3 = data[2];
	switch (op) {
		case 1:
			data[1] = el2;
			data[0] = el1;
			break;
		case 2:
			data[1] = el3;
			data[2] = el2;
			break;
		case 3:
			data[0] = el3;
			data[2] = el1;
			break;
		case 4:
			data[0] = el3;
			data[1] = el1;
			data[2] = el2;
			break;
		case 5:
			data[2] = el1;
			data[0] = el2;
			data[1] = el3;
			break;
	}
}
