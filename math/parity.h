#include "representation/rubiksmap.h"
#include "permutation.h"

// returns 0 if valid, 1 on parity swap, 2 on edge orientations, 3 on corner orientations
int validate_map(RubiksMap * map);

// returns 0 for even parity, 1 for odd parity
int cube_edge_swap_parity(const uint8_t * edges);
int cube_corner_swap_parity(const uint8_t * corners);

// validations return 1 if valid or 0 if not
int validate_corner_orientation(const uint8_t * corners);
int validate_edge_orientation(uint16_t edgeInfo);
