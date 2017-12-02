#ifndef mvl_linear_backproject_h_
#define mvl_linear_backproject_h_

// not used? #include <iostream>
// not used? #include <vcl_compiler.h>
#include <iostream>
#include <cmath>
#include <vector>

#include <mvl/PMatrix.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>

#include <vnl/vnl_vector.h>

// Author: Oli Cooper.
// For details see Hartley & Sturm 'Triangulation'
// To use iterative method call initially with re_weighted = false, then repeat
// passing previous value of X and re_weighted = true
// repeat until convergence achieved (compare weights vector returned against previous)
// Least-Squares method is affine invariant whereas eigenvector method is not

vnl_vector<double> mvl_linear_backproject(
          // camera matrices:
          std::vector<PMatrix> Ps,
          // image coordinates:
          HomgPoint2D const *imgcoords,
          // world point:
          HomgPoint3D &X,
          // if true use current value of X to commute weights.
          bool re_weighted = false,
          // if 'e' then use eigenvalue method if 'l' use least-squares
          char method = 'e');

#endif // mvl_linear_backproject_h_
