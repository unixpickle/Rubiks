Indexes
=======

The `indexmaker` program can be used to generate cached tables of specific Rubik's Cube states. These tables record a specific configuration with respect to a sub-portion of the Rubik's Cube, and this configuration is associated to the minimum number of moves required to attain the state. Tables like this can be used for several purposes, but for me they will play the role of a heuristic in a more generalized Rubik's Cube search.

Usage
=====

Using the `indexformat` command could not be simpler. Here are some examples:

    ./indexmaker corners.anci 11 corners

This will search all corner positions which can be reached in 11 moves or less.

    ./indexmaker edgefront.anci 9 edgefront

This will search all front edge positions which can be reached in 9 moves or less. Note that all front corner situations can be reached in *10* moves or less, so a few configurations will not be included in the file.

Format
======

The indexer outputs Alex Nichol Cube Index 2.0 file. The format is very straightforward:

 * The first 4 bytes are "ANC2", the format signature
 * The next byte indicates the sub-problem type. A table is included below.
 * The next byte indicates the maximum number of moves required by any configuration in the file.
 * The data which follows this header consists of sub-problem specific index data. The last byte of each entry in this data is a uint8 indicating the state's move count.

Sub-problems:
 * Front edges (0x1) - the front 4 edges and the two edges on the top face of the Rubik's Cube. 6 bytes are used for each entry's index data.
 * Back edges (0x2) - the back 4 and the bottom 2 edges. 6 bytes are used for each entry's index data.
 * All edges (0x3) - all 12 edges on the cube. 12 bytes per entry.
 * Corners (0x4) - all 8 corners of the Rubik's Cube. 8 bytes per entry.
