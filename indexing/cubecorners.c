#include "cubecorners.h"

#define kCubeCornerOperationCount 18

static int foundTotal = 0;
static long tableMem = 0;

static const CubeCornerIndices[] = {2, 26, 0, 24, 30, 6, 32, 8, 5, 29, 3, 27, 11, 35, 9, 33, 39, 51, 41, 52, 50, 38, 48, 36};

static void _cubecorners_recursive_search(int depth, int maxDepth, RubiksMap * nodeMap, RubiksMap ** operations, TableNode * root);

TableNode * cubecorners_fewest_moves_table(RubiksMap * baseMap) {
	TableNode * baseTable = (TableNode *)malloc(sizeof(TableNode));
	RubiksMap ** operations = malloc(sizeof(RubiksMap *) * kCubeCornerOperationCount);
	operations[0] = rubiks_map_create_top();
	operations[1] = rubiks_map_create_bottom();
	operations[2] = rubiks_map_create_right();
	operations[3] = rubiks_map_create_left();
	operations[4] = rubiks_map_create_front();
	operations[5] = rubiks_map_create_back();
	int i;
	for (i = 0; i < 6; i++) {
		RubiksMap * doubleTurn = rubiks_map_new_identity();
		rubiks_map_multiply(doubleTurn, operations[i], operations[i]);
		operations[i + 6] = rubiks_map_inverse(operations[i]);
		operations[i + 12] = doubleTurn;
	}
	for (i = 1; i < 12; i++) {
		printf("Performing search with depth of %d\n", i);
		_cubecorners_recursive_search(0, i, baseMap, operations, baseTable);
	}
	for (i = 0; i < kCubeCornerOperationCount; i++) {
		rubiks_map_free(operations[i]);
	}
	free(operations);
	return baseTable;
}

// table manipulation

TableNode * cubecorners_table_find_cube(TableNode * root, RubiksMap * map) {
	TableNode * currentNode = root;
	int i = 0;
	for (i = 0; i < 24; i++) {
		int subIndex = map->indices[CubeCornerIndices[i]] - 1;
		void * nextPtr = currentNode->subnodes[subIndex];
		if (!nextPtr) return NULL;
		currentNode = (TableNode *)nextPtr;
	}
	return currentNode;
}

int cubecorners_table_moves_for_cube(TableNode * root, RubiksMap * map) {
	TableNode * node = cubecorners_table_find_cube(root, map);
	if (!node) return -1;
	return node->moveCount;
}

void cubecorners_table_add_map(TableNode * root, RubiksMap * map, int moves, int maxDepth) {
	TableNode * currentNode = root;
	int i = 0;
	for (i = 0; i < 24; i++) {
		int subIndex = map->indices[CubeCornerIndices[i]] - 1;
		void * nextPtr = currentNode->subnodes[subIndex];
		if (!nextPtr) {
			// begin construction
			int j;
			for (j = i; j < 24; j++) {
				tableMem += sizeof(TableNode);
				TableNode * node = (TableNode *)malloc(sizeof(TableNode));
				bzero(node, sizeof(TableNode));
				int subnodeIndex = map->indices[CubeCornerIndices[j]] - 1;
				currentNode->subnodes[subnodeIndex] = node;
				if (j == 23) {
					node->moveCount = moves;
					node->maxDepth = maxDepth;
				} else {
					node->moveCount = -1;
					node->maxDepth = -1;
					currentNode = node;
				}
			}
			return;
		}
		currentNode = (TableNode *)nextPtr;
	}
}

// search queue

static void _cubecorners_recursive_search(int depth, int maxDepth, RubiksMap * nodeMap, RubiksMap ** operations, TableNode * root) {
	TableNode * node = cubecorners_table_find_cube(root, nodeMap);
	if (node) {
		if (node->moveCount < depth) return;
		if (node->maxDepth >= maxDepth) return;
	} else {
		cubecorners_table_add_map(root, nodeMap, depth, maxDepth);
		foundTotal += 1;
		if (foundTotal % (1<<16) == 0) {
			printf("%d found, mem %ld\n", foundTotal, tableMem);
		}
	}
	if (depth == maxDepth) return;
	int i;
	RubiksMap * deeperCube = rubiks_map_new_identity();
	for (i = 0; i < kCubeCornerOperationCount; i++) {
		rubiks_map_multiply(deeperCube, operations[i], nodeMap);
		_cubecorners_recursive_search(depth + 1, maxDepth, deeperCube, operations, root);
	}
	rubiks_map_free(deeperCube);
}
