Korf Solver
===========

This is the primary solver that is shipped with this project.

Usage
=====

This demonstration assumes that you have created or downloaded all three indexes and that you have placed them in a `tables/` directory.

    ./solvesearch tables/edgeback.anc2 tables/edgefront.anc2 tables/corners.anc2 20 8

This will search on 8 threads, and it will not terminate its search until it has tried all possible solutions with 20 moves or less.  The order of the index files in the arguments list does matter.
