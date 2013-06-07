#include "shard.h"

#define kCacheTableShardBufferSize 512

static long long _entry_compare(const unsigned char * b1, const unsigned char * b2, long long len);
static void _shard_base_insert_at(ShardNode * node, const unsigned char * data, long long dataLength, long long index);
static void _shard_node_insert_subnode(ShardNode * node, ShardNode * subnode, long long index);

ShardNode * shard_node_new(long long depthRem) {
    ShardNode * node = (ShardNode *)malloc(sizeof(ShardNode));
    bzero((void *)node, sizeof(ShardNode));
    node->depthRemaining = depthRem;
    return node;
}

void shard_node_free(ShardNode * node) {
    if (node->depthRemaining == 0) {
        free(node->nodeData);
        if (node->subnodes) {
            free(node->subnodes);
        }
    } else {
        long long i;
        for (i = 0; i < node->subnodeCount; i++) {
            shard_node_free(node->subnodes[i]);
        }
        free(node->subnodes);
        if (node->nodeData) {
            free(node->nodeData);
        }
    }
    free(node);
}

ShardNode * shard_node_search_base(ShardNode * root, 
                                   const unsigned char * data,
                                   long long entrySize, long long create) {
    if (entrySize == 0) return NULL;
    if (root->depthRemaining == 0) return root;
    // find the next subnode down using a binary search
    long long highIndex = root->subnodeCount;
    long long lowIndex = -1;
    ShardNode * testNode = NULL;
    while (highIndex - lowIndex > 1) {
        long long testIndex = (highIndex + lowIndex) / 2;
        testNode = (ShardNode *)root->subnodes[testIndex];
        if (testNode->nodeCharacter == data[0]) {
            highIndex = testIndex;
            lowIndex = testIndex;
            break;
        } else if (testNode->nodeCharacter < data[0]) {
            lowIndex = testIndex;
        } else if (testNode->nodeCharacter > data[0]) {
            highIndex = testIndex;
        }
    }
    long long nodeIndex = (highIndex + lowIndex) / 2;
    if (nodeIndex >= 0 && nodeIndex < root->subnodeCount) {
        testNode = (ShardNode *)root->subnodes[nodeIndex];
        if (testNode->nodeCharacter == data[0]) {
            return shard_node_search_base(testNode, &data[1], 
                                          entrySize - 1, create);
        } else if (testNode->nodeCharacter < data[0]) {
            nodeIndex++;
        }
    }
    if (!create) return NULL;
    if (nodeIndex < 0) nodeIndex = 0;
    // we have to create the rest of the nodes down
    ShardNode * nextNode = (ShardNode *)malloc(sizeof(ShardNode));
    bzero(nextNode, sizeof(ShardNode));
    nextNode->depthRemaining = root->depthRemaining - 1;
    nextNode->nodeCharacter = data[0];
    _shard_node_insert_subnode(root, nextNode, nodeIndex);
    return shard_node_search_base(nextNode, &data[1], entrySize - 1, create);
}

int shard_node_base_add(ShardNode * node, 
                        const unsigned char * data, 
                        long long entrySize, 
                        long long footerSize) {
    if (!node->nodeData) {
        node->nodeData = (unsigned char *)malloc(kCacheTableShardBufferSize * entrySize);
        node->nodeDataAlloc = kCacheTableShardBufferSize;
        node->nodeDataSize = 1;
        memcpy(node->nodeData, data, entrySize);
        return 1;
    }
    // perform a binary search to find the insertion index
    long long lowestIndex = -1;
    long long highestIndex = node->nodeDataSize;
    while (highestIndex - lowestIndex > 1) {
        long long testIndex = (highestIndex + lowestIndex) / 2;
        const unsigned char * testObject = &node->nodeData[testIndex * entrySize];
        long long relativity = _entry_compare(testObject, data,
                                         entrySize - footerSize);
        if (relativity == 1) {
            highestIndex = testIndex;
        } else if (relativity == -1) {
            lowestIndex = testIndex;
        } else {
            lowestIndex = testIndex;
            highestIndex = testIndex;
            break;
        }
    }
    long long insertIndex = (highestIndex + lowestIndex) / 2;
    if (insertIndex < 0) insertIndex = 0;
    else if (insertIndex > node->nodeDataSize) insertIndex--;
    else {
        unsigned char * closeObject = &node->nodeData[insertIndex * entrySize];
        long long relativity = _entry_compare(closeObject, data, 
                                       entrySize - footerSize);
        if (relativity == 0) return 0;
        else if (relativity < 0) insertIndex++;
    }
    
    _shard_base_insert_at(node, data, entrySize, insertIndex);
    return 1;
}

unsigned char * shard_node_base_lookup(ShardNode * node,
                                       const unsigned char * data,
                                       long long entrySize,
                                       long long footerSize) {
    if (!node->nodeData || !node->nodeDataSize) return NULL;
    long long lowestIndex = -1;
    long long highestIndex = node->nodeDataSize;
    while (highestIndex - lowestIndex > 1) {
        long long testIndex = (highestIndex + lowestIndex) / 2;
        const unsigned char * testObject = &node->nodeData[testIndex * entrySize];
        long long relativity = _entry_compare(testObject, data,
                                        entrySize - footerSize);
        if (relativity == 1) {
            highestIndex = testIndex;
        } else if (relativity == -1) {
            lowestIndex = testIndex;
        } else {
            lowestIndex = testIndex;
            highestIndex = testIndex;
            break;
        }
    }
    long long bestIndex = (highestIndex + lowestIndex) / 2;
    if (bestIndex < 0) return NULL;
    if (bestIndex >= node->nodeDataSize) return NULL;
    unsigned char * closeObject = &node->nodeData[bestIndex * entrySize];
    long long relativity = _entry_compare(closeObject, data, 
                                    entrySize - footerSize);
    if (relativity == 0) return closeObject;
    return NULL;
}

/////////////
// PRIVATE //
/////////////

static void _shard_base_insert_at(ShardNode * node, 
                                  const unsigned char * data, 
                                  long long dataLength, long long index) {
    if (node->nodeDataSize >= node->nodeDataAlloc) {
        node->nodeDataAlloc += kCacheTableShardBufferSize;
        node->nodeData = realloc(node->nodeData, node->nodeDataAlloc * dataLength);
    }
    long long copyCount = node->nodeDataSize - index;
    if (copyCount > 0) {
        unsigned char * memStart = &node->nodeData[index * dataLength];
        unsigned char * memDest = &node->nodeData[(index + 1) * dataLength];
        long long memSize = copyCount * dataLength;
        memmove(memDest, memStart, memSize);
    }
    memcpy(&node->nodeData[index * dataLength], data, dataLength);
    node->nodeDataSize++;
}

static void _shard_node_insert_subnode(ShardNode * node, ShardNode * subnode, long long index) {
    if (!node->subnodes) {
        node->subnodes = malloc(sizeof(void *) * 8);
        node->subnodeAlloc = 8;
    } else if (node->subnodeAlloc == node->subnodeCount) {
        node->subnodeAlloc += 8;
        node->subnodes = realloc(node->subnodes, sizeof(void *) * node->subnodeAlloc);
    }
    long long moveCount = node->subnodeCount - index;
    if (moveCount > 0) {
        void * memStart = (void *)&node->subnodes[index];
        void * memDest = (void *)&node->subnodes[index + 1];
        long long memSize = moveCount * sizeof(void *);
        memmove(memDest, memStart, memSize);
    }
    node->subnodes[index] = subnode;
    node->subnodeCount++;
}


static long long _entry_compare(const unsigned char * b1, const unsigned char * b2, long long len) {
    long long i;
    for (i = 0; i < len; i++) {
        if (b1[i] > b2[i]) return 1;
        if (b1[i] < b2[i]) return -1;
    }
    return 0;
}
