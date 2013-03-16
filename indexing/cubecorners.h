#include "cube.h"

typedef struct {
	char moveCount; // used on lowermost node
	char maxDepth;
	void * subnodes[6];
} TableNode;

TableNode * cubecorners_fewest_moves_table(RubiksMap * baseMap);
TableNode * cubecorners_table_find_cube(TableNode * root, RubiksMap * map);
int cubecorners_table_moves_for_cube(TableNode * root, RubiksMap * map);
void cubecorners_table_add_map(TableNode * root, RubiksMap * map, int moves, int maxDepth);
