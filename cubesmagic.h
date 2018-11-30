#pragma once

/*
Before the algorithm:
1. Define space : (minx, maxx, miny, maxy, minz, maxz) by pulling these values from the cloud point
2. Subdivide space : either number of cells or the step size on each axis.
3. Assign a value to each vertex of each box.
(the above is done outside of the algorithm, the steps below are what the algorithm actually does)
*/

