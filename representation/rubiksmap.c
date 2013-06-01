#include "rubiksmap.h"
#include "stickerindices.h"
#include "symmetry.h"

static const char _IdentityMapData[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

// optimized rubiks operation methods
const static struct {
    int cornerIndices[4];
    int edgeIndices[4];
    int cornerDestinations[4];
    int edgeDestinations[4];
    int symmetry;
} _OperationTable[] = {
    // top
    {{0x2, 0x3, 0x7, 0x6},
        {0, 4, 6, 5},
        {0x6, 0x2, 0x3, 0x7},
        {4, 6, 5, 0},
        3},
    // bottom
    {{0x0, 0x1, 0x5, 0x4},
        {2, 10, 8, 11},
        {0x4, 0x0, 0x1, 0x5},
        {10, 8, 11, 2},
        3},
    // right
    {{0x7, 0x6, 0x4, 0x5},
        {11, 1, 5, 7},
        {0x5, 0x7, 0x6, 0x4},
        {7, 11, 1, 5},
        2},
    // left
    {{0x3, 0x2, 0x0, 0x1},
        {10, 3, 4, 9},
        {0x1, 0x3, 0x2, 0x0},
        {9, 10, 3, 4},
        2},
    // front
    {{0x3, 0x7, 0x5, 0x1},
        {0, 1, 2, 3},
        {0x7, 0x5, 0x1, 0x3},
        {1, 2, 3, 0},
        1},
    // back
    {{0x2, 0x6, 0x4, 0x0},
        {6, 7, 8, 9},
        {0x6, 0x4, 0x0, 0x2},
        {7, 8, 9, 6},
        1}
};

RubiksMap * rubiks_map_user_input() {
    StickerMap * stickers = sticker_map_user_input();
    if (!stickers) return NULL;
    RubiksMap * map = rubiks_map_from_sticker_map(stickers);
    sticker_map_free(stickers);
    return map;
}

RubiksMap * rubiks_map_user_input_premoves(RubiksMap * premoves) {
    StickerMap * stickers = sticker_map_user_input();
    if (!stickers) return NULL;
    RubiksMap * map = rubiks_map_from_sticker_map(stickers);
    sticker_map_free(stickers);
    if (!premoves) return map;
    RubiksMap * newMap = rubiks_map_new_identity();
    rubiks_map_multiply(newMap, premoves, map);
    rubiks_map_free(map);
    return newMap;
}

RubiksMap * rubiks_map_new_identity() {
    RubiksMap * map = (RubiksMap *)malloc(sizeof(RubiksMap));
    memcpy(map->pieces, _IdentityMapData, 20);
    return map;
}

void rubiks_map_copy_into(RubiksMap * out, const RubiksMap * input) {
    memcpy(out->pieces, input->pieces, 20);
}

RubiksMap * rubiks_map_inverse(RubiksMap * map) {
    RubiksMap * inverse = (RubiksMap *)malloc(sizeof(RubiksMap));
    int i;
    for (i = 0; i < 8; i++) {
        unsigned char piece = map->pieces[i];
        int sourceIndex = piece & 7;
        unsigned char inv = symmetry_operation_inverse((piece >> 4) & 7);
        inverse->pieces[sourceIndex] = i | (inv << 4);
    }
    for (i = 0; i < 12; i++) {
        unsigned char piece = map->pieces[i + 8];
        int sourceIndex = piece & 15;
        unsigned char inv = symmetry_operation_inverse((piece >> 4) & 7);
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
    // multiply the edges
    for (i = 0; i < 12; i++) {
        unsigned char leftPiece = left->pieces[i + 8];
		if (leftPiece == i) { // identity piece
			out->pieces[i + 8] = right->pieces[i + 8];
			continue;
		}
        int fromIndex = leftPiece & 15;
        unsigned char piece = right->pieces[fromIndex + 8];
        int orientation = (piece >> 4) & 7;
        int leftOrientation = (leftPiece >> 4) & 7;
        orientation = symmetry_operation_compose(leftOrientation, orientation);
        out->pieces[i + 8] = (piece & 15) | (orientation << 4);
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
        int newSym = symmetry_operation_compose(symmetry, (piece >> 4) & 7);
        out->pieces[destIndex] = (piece & 7) | (newSym << 4);
    }
    for (i = 0; i < 4; i++) {
        int sourceIndex = _OperationTable[o].edgeIndices[i];
        int destIndex = _OperationTable[o].edgeDestinations[i];
        unsigned char piece = map->pieces[sourceIndex + 8];
        int newSym = symmetry_operation_compose(symmetry, (piece >> 4) & 7);
        out->pieces[destIndex + 8] = (piece & 15) | (newSym << 4);
    }
}

int rubiks_map_is_identity(RubiksMap * map) {
    return (memcmp(map->pieces, _IdentityMapData, 20) == 0);
}

uint16_t rubiks_map_edge_orientations(RubiksMap * map) {
    uint16_t bitArray = 0;
    int i;
    for (i = 0; i < 12; i++) {
        unsigned char piece = map->pieces[i + 8];
        const char * pieceIndices = EdgeIndices[i];
        unsigned char colors[3];
        memcpy(colors, EdgePieces[piece & 15], 3);
        symmetry_operation_perform((piece >> 4) & 7, colors);
        // check the edge orientation
        uint16_t mask = 1 << i;
        int isTopBottomEdge = 0;
        int sigColor = colors[1] == 0 ? colors[2] : colors[1];
        int j;
        for (j = 0; j < 3; j++) {
            if (colors[j] == 3 || colors[j] == 4) {
                isTopBottomEdge = 1;
            }
        }
        // two conditions under which it is a good edge
        if (isTopBottomEdge && (sigColor == 3 || sigColor == 4)) {
            bitArray |= mask;
        } else if (!isTopBottomEdge && (sigColor == 1 || sigColor == 2)) {
            bitArray |= mask;
        }
    }
    return bitArray;
}

uint16_t rubiks_map_corner_orientations(RubiksMap * map) {
    uint16_t cornersInfo = 0;
    int i;
    for (i = 0; i < 8; i++) {
        unsigned char piece = map->pieces[i];
        const unsigned char * pieceIndices = CornerIndices[i];
        unsigned char colors[3];
        memcpy(colors, CornerPieces[piece & 7], 3);
        symmetry_operation_perform((piece >> 4) & 7, colors);
        uint16_t orientation = 0;
        if (colors[1] == 3 || colors[1] == 4) orientation = 1;
        else if (colors[2] == 3 || colors[2] == 4) orientation = 2;
        cornersInfo |= orientation << (i * 2);
    }
    return cornersInfo;
}

uint16_t rubiks_map_topbottom_edge_map(RubiksMap * map) {
    uint16_t edgeTypes = 0;
    int i;
    for (i = 0; i < 12; i++) {
        int pieceNumber = map->pieces[i + 8] & 0xf;
        if (pieceNumber != 1 && pieceNumber != 3 &&
            pieceNumber != 9 && pieceNumber != 7) {
            edgeTypes |= 1 << i;
        }
    }
    return edgeTypes;
}

void rubiks_map_free(RubiksMap * map) {
    free(map);
}

/////////////////
// CONVERSIONS //
/////////////////

RubiksMap * rubiks_map_from_sticker_map(StickerMap * stickers) {
    // doing a task like this requires a LOT of patients...and organization
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
            symmetry = symmetry_operation_find(CornerPieces[j], realColors);
            if (symmetry >= 0) {
                realIndex = j;
                break;
            }
        }
        if (realIndex < 0) {
            printf("not found: ");
            for (j = 0; j < 3; j++) {
                printf("%d ", realColors[j]);
            }
            printf("\n");
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
            realColors[j] = stickers->indices[index];
        }
        int realIndex = -1, symmetry = -1;
        for (j = 0; j < 12; j++) {
            symmetry = symmetry_operation_find(EdgePieces[j], realColors);
            if (symmetry >= 0) {
                realIndex = j;
                break;
            }
        }
        if (realIndex < 0) {
            printf("not found edges (%d): ", i);
            for (j = 0; j < 3; j++) {
                printf("%d ", realColors[j]);
            }
            printf("\n");
            rubiks_map_free(map);
            return NULL;
        }
        map->pieces[i + 8] = realIndex | (symmetry << 4);
    }
    return map;
}

StickerMap * rubiks_map_to_sticker_map(RubiksMap * map) {
    StickerMap * output = sticker_map_identity();
    int i,j;
    // corner piece conversions
    for (i = 0; i < 8; i++) {
        unsigned char piece = map->pieces[i];
        const unsigned char * pieceIndices = CornerIndices[i];
        unsigned char colors[3];
        memcpy(colors, CornerPieces[piece & 7], 3);
        symmetry_operation_perform((piece >> 4) & 7, colors);
        for (j = 0; j < 3; j++) {
            output->indices[pieceIndices[j]] = colors[j];
        }
    }
    for (i = 0; i < 12; i++) {
        unsigned char piece = map->pieces[i + 8];
        const char * pieceIndices = EdgeIndices[i];
        unsigned char colors[3];
        memcpy(colors, EdgePieces[piece & 15], 3);
        symmetry_operation_perform((piece >> 4) & 7, colors);
        for (j = 0; j < 3; j++) {
            if (pieceIndices[j] < 0) continue;
            output->indices[pieceIndices[j]] = colors[j];
        }
    }
    return output;
}

