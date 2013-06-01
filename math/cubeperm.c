#include "cubeperm.h"

static const struct {
    int initIndex, endIndex;
    int symmetry;
} EdgeNULLSymmetryMap[] = {
    {0, 1, 1},
    {0, 2, 3},
    {1, 2, 2}
};

static const struct {
    int nullIndex;
    int symmetry;
} EdgeFlipSymmetryMap[] = {
    {0, 2},
    {1, 3},
    {2, 1}
};

// possible orientation symmetries for a piece
int cube_perm_corner_coset(int pieceIndex, int destination) {
    int i, numCommon = 0;
    for (i = 0; i < 3; i++) {
        int pieceValue = (pieceIndex & (4 >> i)) != 0;
        int destValue = (destination & (4 >> i)) != 0;
        if (pieceValue == destValue) numCommon++;
    }
    if (numCommon % 2 == 0) {
        // an even translation means the piece must be modified:
        // the coset we return is the result of a (1 2 3 -> 2 1 3)
        return 1;
    }
    // the identity coset is where it's at, man
    return 0;
}

void cube_perm_edge_symmetries(int pieceIndex, int destination, int * or1, int * or2) {
    unsigned char indicesPiece[3], indicesDest[3];
    memcpy(indicesPiece, EdgePieces[pieceIndex], 3);
    memcpy(indicesDest, EdgePieces[destination], 3);
    int permSymmetry = 0, flipSymmetry, i, nullPiece, nullDest;
    for (i = 0; i < 3; i++) {
        if (indicesPiece[i] == 0) nullPiece = i;
        if (indicesDest[i] == 0) nullDest = i;
    }
    // find the appropriate permutation symmetry
    for (i = 0; i < 3; i++) {
        int index1 = EdgeNULLSymmetryMap[i].initIndex;
        int index2 = EdgeNULLSymmetryMap[i].endIndex;
        if ((index1 == nullPiece && index2 == nullDest) ||
            (index2 == nullPiece && index1 == nullDest)) {
            permSymmetry = EdgeNULLSymmetryMap[i].symmetry;
        }
        if (EdgeFlipSymmetryMap[i].nullIndex == nullDest) {
            flipSymmetry = EdgeFlipSymmetryMap[i].symmetry;
        }
    }
    
    // our two outputs:
    
    *or1 = permSymmetry;
    
    // flip the symmetry in the other possible orientation
    *or2 = symmetry_operation_compose(flipSymmetry, permSymmetry);
}

// composition of permutations
void cube_perm_exp_corners(uint8_t * out, const uint8_t * corners, int power) {
    memcpy(out, corners, 8);
    int i;
    for (i = 1; i < power; i++) {
        int j;
        uint8_t temp[8];
        memcpy(temp, out, 8);
        for (j = 0; j < 8; j++) {
            // grab a value, apply a symmetry to it, and pop it in our new map
            int grabIndex = corners[j] & 0xf;
            int symmetry = (corners[j] >> 4) & 0xf;
            int grabValue = temp[grabIndex];
            int grabSym = (grabValue >> 4) & 0xf;
            grabSym = symmetry_operation_compose(symmetry, grabSym);
            grabValue = (grabValue & 0xf) | (grabSym << 4);
            out[j] = grabValue;
        }
    }
}

void cube_perm_exp_edges(uint8_t * out, const uint8_t * edges, int power) {
    memcpy(out, edges, 12);
    int i;
    for (i = 1; i < power; i++) {
        int j;
        uint8_t temp[12];
        memcpy(temp, out, 12);
        for (j = 0; j < 12; j++) {
            // grab a value, apply a symmetry to it, and pop it in our new map
            int grabIndex = edges[j] & 0xf;
            int symmetry = (edges[j] >> 4) & 0xf;
            int grabValue = temp[grabIndex];
            int grabSym = (grabValue >> 4) & 0xf;
            grabSym = symmetry_operation_compose(symmetry, grabSym);
            grabValue = (grabValue & 0xf) | (grabSym << 4);
            out[j] = grabValue;
        }
    }
}
