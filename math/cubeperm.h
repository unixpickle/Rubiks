#include "representation/rubiksmap.h"
#include "representation/stickerindices.h"
#include <stdint.h>

// possible orientation symmetries for a piece
int cube_perm_corner_coset(int pieceIndex, int destination);
void cube_perm_edge_symmetries(int pieceIndex, int destination, int * or1, int * or2);

// composition of permutations
void cube_perm_exp_corners(uint8_t * out, const uint8_t * corners, int power);
void cube_perm_exp_edges(uint8_t * out, const uint8_t * edges, int power);
