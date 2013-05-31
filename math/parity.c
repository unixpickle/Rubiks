#include "parity.h"

int CornerChain[] = {0, 1, 3, 2, 6, 7, 5, 4};

static int is_symmetry_z_invariant(int sym);

int validate_map(RubiksMap * map) {
    int par1 = cube_edge_swap_parity(&map->pieces[8]);
    int par2 = cube_corner_swap_parity(map->pieces);
    if (par1 != par2) return 1;
    uint16_t edgeOr = rubiks_map_edge_orientations(map);
    if (!validate_edge_orientation(edgeOr)) return 2;
    if (!validate_corner_orientation(map->pieces)) return 3;
    return 0;
}

// returns 0 for even parity, 1 for odd parity
int cube_edge_swap_parity(const uint8_t * edges) {
    Perm * p = perm_from_bytes(edges, 0xf, 12);
    int parity = perm_parity(p);
    perm_free(p);
    return parity;
}

int cube_corner_swap_parity(const uint8_t * corners) {
    Perm * p = perm_from_bytes(corners, 0xf, 8);
    int parity = perm_parity(p);
    perm_free(p);
    return parity;
}

// validations return 1 if valid or 0 if not
int validate_corner_orientation(const uint8_t * _corners) {
    uint8_t corners[8];
    memcpy(corners, _corners, 8);
    int i;
    for (i = 0; i < 7; i++) {
        int fixCorner = CornerChain[i];
        int otherCorner = CornerChain[i + 1];
        int sym1 = (corners[fixCorner] >> 4) & 0xf;
        int sym2 = (corners[otherCorner] >> 4) & 0xf;
        while (!is_symmetry_z_invariant(sym1)) {
            sym1 = symmetry_operation_compose(4, sym1);
            sym2 = symmetry_operation_compose(4, sym2);
        }
        corners[fixCorner] = (corners[fixCorner] & 0xf) | (sym1 << 4);
        corners[otherCorner] = (corners[otherCorner] & 0xf) | (sym2 << 4);
    }
    if (is_symmetry_z_invariant((corners[CornerChain[7]] >> 4) & 0xf)) {
        return 1;
    }
    return 0;
}

int validate_edge_orientation(uint16_t orientations) {
    int numGood = 0;
    int i;
    for (i = 0; i < 12; i++) {
        if ((orientations & (1 << i)) != 0) {
            numGood++;
        }
    }
    if (numGood % 2 != 0) return 0;
    return 1;
}

/***********
 * PRIVATE *
 ***********/

static int is_symmetry_z_invariant(int sym) {
    return (sym == 1 || sym == 0);
}
