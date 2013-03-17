/**
 * This API was designed to count the minimum number of moves
 * required to achieve various configurations of a subset
 * of the full 3x3x3 rubik's cube data.
 */

#include "cube.h"

typedef struct {
	int significantIndexCount;
	int indexTableDepth;
    int maximumDepth;
	RubiksMap * baseMap;
	unsigned const char * significantIndices;
} CCUserInfo;

typedef struct {
	unsigned char * rawEntries; // raw entries buffer
	int rawEntriesCount; // number of raw entries currently used
	int rawEntriesAlloc; // number of raw entries currently allocated
	void * subnodes[6];
} CCTableNode;

// retreiving and working with tables
/**
 * Computes a full table of all the possible configurations of the map
 * along the given significant indices.
 */
CCTableNode * cc_compute_table(CCUserInfo info);

/**
 * Traverses a tree of table nodes until finding the lowest node
 * which would contain the map in its rawEntries.
 * If the root contains no node for this, NULL will be returned.
 */
CCTableNode * cc_table_node_for_map(CCTableNode * root,
                                    CCUserInfo * userInfo,
                                    RubiksMap * map);

/**
 * Searches a table node's rawEntries data for a given map
 * and returns the moves and indices for this map.
 */
int cc_table_entry_for_node(CCTableNode * node,
                            CCUserInfo * userInfo,
                            RubiksMap * map,
                            unsigned char * indicesOut,
                            unsigned char * movesOut,
                            unsigned char * movesCountOut,
                            unsigned char * searchDepthOut);

// modification of tables

/**
 * Adds a map to a table node tree. This method does a LOT
 * for you. Be happy.
 */
void cc_table_add_entry(CCTableNode * root,
                        CCUserInfo * userInfo,
                        RubiksMap * map,
                        unsigned char * moves,
                        unsigned char movesCount,
                        unsigned char searchDepth);
