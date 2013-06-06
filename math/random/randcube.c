#include "randcube.h"

static int _generate_corners(uint8_t * cornersOut);
static void _generate_edges(uint8_t * edgesOut, int cornerParity);
static int validate_edges(const uint8_t * edges);

static int _generate_corners(uint8_t * cornersOut) {
    Perm * p = rand_perm(8);
    
    srand(time(NULL));
    
    int i;
    for (i = 0; i < 8; i++) {
        int piece = p->map[i];
        int coset = cube_perm_corner_coset(piece, i);
        int perm = rand() % 3;
        if (perm != 0) perm += 3; // perms are 0, 4, and 5
        int orientation = symmetry_operation_compose(coset, perm);
        cornersOut[i] = piece | (orientation << 4);
    }
    
    while (!validate_corner_orientation(cornersOut)) {
        int sym = (cornersOut[7] >> 4) & 7;
        sym = symmetry_operation_compose(sym, 4);
        cornersOut[7] &= 0xf;
        cornersOut[7] |= (sym << 4);
    }
    
    int parity = perm_parity(p);
    perm_free(p);
    return parity;
}

static void _generate_edges(uint8_t * edgesOut, int cornerParity) {
    Perm * p = rand_perm(12);
    if (perm_parity(p) != cornerParity) {
        rand_change_parity(p);
    }
    uint16_t edgeOrientations = 0;
    int i;
    for (i = 0; i < 12; i++) {
        int piece = p->map[i];
        int s1, s2;
        cube_perm_edge_symmetries(piece, i, &s1, &s2);
        int sym = rand() % 2 == 0 ? s1 : s2;
        edgesOut[i] = piece | (sym << 4);
        if (i == 11) {
            if (!validate_edges(edgesOut)) {
                int newSym = sym == s1 ? s2 : s1;
                edgesOut[i] = piece | (newSym << 4);
            }
        }
    }
    
    perm_free(p);
}

RubiksMap * rand_rubiks_map() {
    uint8_t corners[8];
    uint8_t edges[12];
    int p = _generate_corners(corners);
    _generate_edges(edges, p);
    RubiksMap * m = rubiks_map_new_identity();
    memcpy(m->pieces, corners, 8);
    memcpy(&m->pieces[8], edges, 12);
    return m;
}

static int validate_edges(const uint8_t * edges) {
    // ugly hack
    RubiksMap * map = rubiks_map_new_identity();
    bzero(map->pieces, 20);
    memcpy(&map->pieces[8], edges, 12);
    uint16_t orientations = rubiks_map_edge_orientations(map);
    rubiks_map_free(map);
    return validate_edge_orientation(orientations);
}
