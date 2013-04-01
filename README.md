Directory Structure
===================

The `representation/` directory includes the fundamental representations of a Rubik's cube.

The `indexing/` directory contains the programs necessary to create indexes for `korfsolver/` and `pocketcube/` to function.

The `korfsolver/` directory is the main solver program which depends on three "pruning tables" to quickly find optimal solutions to a Rubik's cube.

The `pocketcube/` directory includes a program which can be used in conjunction with a corners index file to instantly solve any 2x2x2 configuration.

Entry Format
============

All solvers expect the white face in front and the blue face on top. The colors are then represented by the following digits (I quickly came to memorize them):

 * White - 1
 * Yellow - 2
 * Blue - 3
 * Green - 4
 * Red - 5
 * Orange - 6

To enter a face of the cube, turn the face towards you and type all 9 colors starting from the top left and going across then down (i.e. type the rows side by side).  In order to look at each face correctly, first place the cube so that the white face is in front and blue on top, and then rotate it in the shortest way possible to look directly at the face you wish to enter.  To type the yellow face, turn the cube around the y axis (i.e. don't move the blue or green faces).  These directions should be sufficient to using all of the solvers.

Compiling
=========

On UNIX and Linux platforms, a simply `make` in the root directory will compile everything and create every executable in its respective directory.  Once you have compiled, you may either wish to use the `indexing/` directory to generate indexes, or you may download all of the indexes from my website:

 * [corners.anc2](http://aqnichol.com/apps/rubiks/corners.anc2)
 * [edgeback.anc2](http://aqnichol.com/apps/rubiks/edgeback.anc2)
 * [edgefront.anc2](http://aqnichol.com/apps/rubiks/edgefront.anc2)

These indexes together take up about 1.4 GB of space.

System Requirements
===================

In order for `korfsolver/` to run, at least 1.4 GB of RAM are necessary. This is because the solver loads the index files into memory while performing its search.

On the other hand, the `pocketcube/` solver only requires about 756MB of RAM because it does not require the additional edge indexes that the `korfsolver/` does.  In addition, one can make a shallower corner index for the `pocketcube/` solver if they wish to lighten the system load.
