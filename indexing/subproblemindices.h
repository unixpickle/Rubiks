#ifndef __SUBPROBLEM_INDICES_H__
#define __SUBPROBLEM_INDICES_H__

static const unsigned char BlockEdgeIndices[8][3] = {
    {8, 9, 10}, // (0, 0, 0)
    {2, 3, 10}, // (0, 0, 1)
    {4, 9, 6}, // (0, 1, 0)
    {0, 3, 4}, // (0, 1, 1)
    {11, 7, 8}, // (1, 0, 0)
    {1, 2, 11}, // (1, 0, 1)
    {5, 6, 7}, // (1, 1, 0)
    {0, 1, 5} // (1,1,1)
};

// deprecated constants
static const unsigned char CubeCornerIndices[] = {2, 26, 0, 24, 30, 6, 32, 8, 5, 29, 3, 27, 11, 35, 9, 33, 39, 51, 41, 53, 50, 38, 48, 36};
static const unsigned char CubeFrontIndices[] = {1, 14, 25, 12, 15, 40, 23, 37, 4, 28, 44, 45};
static const unsigned char CubeBackIndices[] = {7, 18, 31, 20, 17, 52, 49, 21, 10, 34, 42, 47};

#endif
