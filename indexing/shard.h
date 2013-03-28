#ifndef __SHARD_H__
#define __SHARD_H__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    void ** subnodes;
    unsigned char subnodeCount;
    unsigned char subnodeAlloc;
    unsigned char nodeCharacter;
    // node data for base nodes
    unsigned char depthRemaining; // 0 means this is a base node
    unsigned char * nodeData;
    int nodeDataSize;
    int nodeDataAlloc;
} ShardNode;

ShardNode * shard_node_new(int depthRem);
void shard_node_free(ShardNode * node);

/**
 * Finds the base node for a given item and creates it
 * if it does not already exist.
 * @argument create  Create the appropriate node if it does
 * not already exist.
 */
ShardNode * shard_node_search_base(ShardNode * root,
                                   const unsigned char * entry,
                                   int entrySize,
                                   int create);

/**
 * Adds an entry to the appropriate base node for it.
 * @argument footerLength  The number of bytes to
 * ignore for comparisons. This must be smaller or
 * equal to the entrySize.
 * @return 1 if the item was added, 0 if the list already
 * contained the element.
 */
int shard_node_base_add(ShardNode * node,
                        const unsigned char * data,
                        int entrySize,
                        int footerLength);

/**
 * Lookup an entry in a base node
 */
unsigned char * shard_node_base_lookup(ShardNode * node,
                                       const unsigned char * data,
                                       int entrySize,
                                       int footerLength);

#endif