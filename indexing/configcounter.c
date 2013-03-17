#include "configcounter.h"

#define kConfigCounterBufferSize 16 // number of entries per allocation
#define kConfigCounterMoveCount 18

static int cubesAdded = 0;

/**
 * 1 = left > right, 0 = equality, -1 = left < right
 */
static int _cc_indices_compare(unsigned char * entry1,
							   unsigned char * entry2,
							   int entryCount);

static void _cc_indices_for_map(CCUserInfo * info,
								RubiksMap * map,
								unsigned char * entryOut);

static int _cc_binary_search(CCUserInfo * info,
							 CCTableNode * node,
							 unsigned char * indices);

static void _cc_node_add_entry(CCTableNode * node,
							   CCUserInfo * userInfo,
							   unsigned char * indices,
							   unsigned char * moves,
							   unsigned char movesCount,
							   unsigned char searchDepth);

static void _cc_recursive_search(CCUserInfo * userInfo,
								 int depth,
								 int maxDepth,
								 char * previousMoves,
								 RubiksMap * nodeMap,
								 RubiksMap ** operations,
								 CCTableNode * root);

static void _cc_indices_print(const unsigned char * indices, int count);

CCTableNode * cc_compute_table(CCUserInfo info) {
	CCTableNode * baseTable = (CCTableNode *)malloc(sizeof(CCTableNode));
	RubiksMap ** operations = malloc(sizeof(RubiksMap *) * kConfigCounterMoveCount);
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
	char previousMoves[1];
	for (i = 0; i <= info.maximumDepth; i++) {
		printf("Performing search with depth of %d\n", i);
		_cc_recursive_search(&info, 0, i, previousMoves,
							 info.baseMap, operations, baseTable);
	}
	for (i = 0; i < kConfigCounterMoveCount; i++) {
		rubiks_map_free(operations[i]);
	}
	free(operations);
	return baseTable;
}

static void _cc_recursive_search(CCUserInfo * userInfo,
								 int depth,
								 int maxDepth,
								 char * previousMoves,
								 RubiksMap * nodeMap,
								 RubiksMap ** operations,
								 CCTableNode * root) {
	CCTableNode * baseNode = cc_table_node_for_map(root, userInfo, nodeMap);
	int exists = 0;
	if (baseNode) {
		unsigned char moveCount = 0;
		unsigned char depthCount = 0;
		unsigned char * depthCountPtr;
	
		exists = cc_table_entry_for_node(baseNode, userInfo, nodeMap,
										 NULL, NULL, &moveCount,
										 &depthCount, &depthCountPtr);
		if (exists) {
			if (moveCount < depth) return;
			if (depthCount == maxDepth && moveCount == depth) return;
			// prevent future nodes from being explored
			*depthCountPtr = maxDepth;
		}
	}
	if (!exists) {
		cc_table_add_entry(root, userInfo, nodeMap, previousMoves, depth, maxDepth);
		cubesAdded += 1;
		if (cubesAdded % 65536 == 0) {
			printf("Found %d cubes\n", cubesAdded);
   		}
	}
	if (depth == maxDepth) return;
	
	// use recursion to explore child nodes
	char * nextPrevious = (char *)malloc(depth + 1);
	if (depth > 0) memcpy(nextPrevious, previousMoves, depth);
	int i;
	RubiksMap * deeperCube = rubiks_map_new_identity();
	for (i = 0; i < kConfigCounterMoveCount; i++) {
		nextPrevious[depth] = i;
		rubiks_map_multiply(deeperCube, operations[i], nodeMap);
		_cc_recursive_search(userInfo,
							 depth + 1,
							 maxDepth,
							 nextPrevious,
							 deeperCube,
							 operations,
							 root);
	}
	free(nextPrevious);
	rubiks_map_free(deeperCube);
}

// looking up nodes

CCTableNode * cc_table_node_for_map(CCTableNode * root,
									CCUserInfo * userInfo,
									RubiksMap * map) {
	char indices[54];
	_cc_indices_for_map(userInfo, map, indices);
	
	int depthCount = userInfo->indexTableDepth;
	CCTableNode * node = root;
	int i;
	for (i = 0; i < depthCount; i++) {
		CCTableNode * nextNode = node->subnodes[indices[i]];
		if (!nextNode) return NULL;
		node = nextNode;
	}
	return node;
}

int cc_table_entry_for_node(CCTableNode * node,
							CCUserInfo * userInfo,
							RubiksMap * map,
							unsigned char * indicesOut,
							unsigned char * movesOut,
							unsigned char * movesCountOut,
							unsigned char * searchDepthOut,
							unsigned char ** searchDepthPtrOut) {
	if (node->rawEntriesCount == 0) return 0; // i.e. no data in node
	
	// get general entry information
	char indices[54];
	int indicesSize = userInfo->significantIndexCount;
	int entryByteSize = userInfo->maximumDepth + indicesSize + 2;
	_cc_indices_for_map(userInfo, map, indices);
	
	// perform the binary search
	int closestIndex = _cc_binary_search(userInfo, node, indices);
	
	// look at the entry which we narrowed in on
	unsigned char * potentialEntry = &node->rawEntries[closestIndex * entryByteSize];
	int finalResult = _cc_indices_compare(potentialEntry, indices, indicesSize);
	if (finalResult) return 0; // the closest entry was not what we search for
	
	// if we made it here, we know we have an exact match.
	
	// copy the indices if the user really wants them (they shouldn't care)
	if (indicesOut) memcpy(indicesOut, potentialEntry, indicesSize);
	
	// the maximum depth is also the number of bytes we use to store the move data
	if (movesOut) memcpy(movesOut, &potentialEntry[indicesSize], userInfo->maximumDepth);
	
	if (movesCountOut) *movesCountOut = potentialEntry[entryByteSize - 2];
	if (searchDepthOut) *searchDepthOut = potentialEntry[entryByteSize - 1];
	if (searchDepthPtrOut) *searchDepthPtrOut = &potentialEntry[entryByteSize - 1];
	return 1; // 1 = happy ending (wow i'm tired)
}

// modification of tables

void cc_table_add_entry(CCTableNode * root,
						CCUserInfo * userInfo,
						RubiksMap * map,
						unsigned char * moves,
						unsigned char movesCount,
						unsigned char searchDepth) {
	char indices[54];
	_cc_indices_for_map(userInfo, map, indices);
	// find the base node for our map, and if it doesn't exist
	// we create it fresh and new.
	int depthCount = userInfo->indexTableDepth;
	CCTableNode * node = root;
	int i;
	for (i = 0; i < depthCount; i++) {
		CCTableNode * nextNode = node->subnodes[indices[i]];
		if (!nextNode) {
			int j;
			for (j = i; j < depthCount; j++) {
				// create the next node
				CCTableNode * newNode = (CCTableNode *)malloc(sizeof(CCTableNode));
				bzero(newNode, sizeof(CCTableNode));
				node->subnodes[indices[j]] = newNode;
				node = newNode;
			}
			break;
		}
		node = nextNode;
	}
	
	// add the actual entry to our node
	_cc_node_add_entry(node, userInfo, indices, moves, movesCount, searchDepth);
}

// private table processing

static int _cc_indices_compare(unsigned char * entry1,
							   unsigned char * entry2,
							   int entryCount) {
	int i;
	for (i = 0; i < entryCount; i++) {
		if (entry1[i] > entry2[i]) {
			return 1;
		} else if (entry1[i] < entry2[i]) {
			return -1;
		}
	}
	return 0;
}

static void _cc_indices_for_map(CCUserInfo * info,
								RubiksMap * map,
								unsigned char * entryOut) {
	int i;
	for (i = 0; i < info->significantIndexCount; i++) {
		entryOut[i] = map->indices[info->significantIndices[i]] - 1;
	}
}

static int _cc_binary_search(CCUserInfo * info,
							 CCTableNode * node,
							 unsigned char * indices) {
	int indicesSize = info->significantIndexCount;
	int entryByteSize = info->maximumDepth + indicesSize + 2;

	int lowestIndex = -1;
	int highestIndex = node->rawEntriesCount;
	while (highestIndex - lowestIndex > 1) {
		int testIndex = (highestIndex + lowestIndex) / 2;
		if (testIndex >= node->rawEntriesCount || testIndex < 0) {
			break;
		}
		unsigned char * testEntry = &node->rawEntries[testIndex * entryByteSize];
		int comparison = _cc_indices_compare(testEntry, indices, indicesSize);
		if (comparison == 0) {
			lowestIndex = testIndex;
			highestIndex = testIndex;
			break;
		} else if (comparison == 1) {
			// testEntry > indices
			highestIndex = testIndex;
		} else {
			// testEntry < indices
			lowestIndex = testIndex;
		}
	}
	int avg = (highestIndex + lowestIndex) / 2;
	if (avg >= node->rawEntriesCount) return node->rawEntriesCount - 1;
	if (avg < 0) return 0;
	return avg;
}

static void _cc_node_add_entry(CCTableNode * node,
							   CCUserInfo * userInfo,
							   unsigned char * indices,
							   unsigned char * moves,
							   unsigned char movesCount,
							   unsigned char searchDepth) {
	// this function was not fun to code
	// that's why I left the task to *tired Alex*
	
	int indicesSize = userInfo->significantIndexCount;
	int entryByteSize = userInfo->maximumDepth + indicesSize + 2;
	
	// special case when this is our first entry in the node
	if (node->rawEntries == NULL) {
		int initialSize = entryByteSize * kConfigCounterBufferSize;
		node->rawEntries = (unsigned char *)malloc(initialSize);
		node->rawEntriesCount = 1;
		node->rawEntriesAlloc = kConfigCounterBufferSize;
		memcpy(node->rawEntries, indices, indicesSize);
		memcpy(&node->rawEntries[indicesSize], moves, movesCount);
		node->rawEntries[entryByteSize - 2] = movesCount;
		node->rawEntries[entryByteSize - 1] = searchDepth;
		return;
	}
	
	// perform a binary search to find the index of insertion
	int insertionIndex = _cc_binary_search(userInfo, node, indices);
	
	// look at the entry which we narrowed in on to make sure it's not equal
	unsigned char * potentialEntry = &node->rawEntries[insertionIndex * entryByteSize];
	int finalResult = _cc_indices_compare(potentialEntry, indices, indicesSize);
	if (!finalResult) return; // the closest entry was identical
	
	// make sure we have enough data room
	if (node->rawEntriesCount == node->rawEntriesAlloc) {
		node->rawEntriesAlloc += kConfigCounterBufferSize;
		int newSize = node->rawEntriesAlloc * entryByteSize;
		node->rawEntries = (unsigned char *)realloc(node->rawEntries, newSize);
	}
	
	if (finalResult == -1) {
		// testEntry < indices
		insertionIndex++;
	}
	int copyCount = node->rawEntriesCount - insertionIndex;
	if (copyCount > 0) {
		memmove(&node->rawEntries[(insertionIndex + 1) * entryByteSize],
				&node->rawEntries[insertionIndex * entryByteSize],
				copyCount * entryByteSize);
	}
	// at first here I forgot *entryByteSize and it baffled me for a long time :'(
	unsigned char * entryPointer = &node->rawEntries[insertionIndex * entryByteSize];
	memcpy(entryPointer, indices, indicesSize);
	memcpy(&entryPointer[indicesSize], moves, movesCount);
	entryPointer[entryByteSize - 2] = movesCount;
	entryPointer[entryByteSize - 1] = searchDepth;
	node->rawEntriesCount += 1;
}

// this was very helpful for debugging purposes
static void _cc_indices_print(const unsigned char * indices, int count) {
	int i;
	for (i = 0; i < count; i++) {
		printf("%d ", (int)indices[i]);
	}
}

