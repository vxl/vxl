#ifdef __GNUC__
#pragma implementation
#endif

//:
//  \file

#include "FDesignMatrix.h"

//: Construct a design matrix for the fundamental matrix least squares problem.
// Specifically, it constructs the matrix
// \f[
// D = \left( \begin{array}{lllllllll}
//          m_{11} m'_{11} & m_{12} m'_{11}  & m_{13} m'_{11} &
//          m_{11} m'_{12} & m_{12} m'_{12}  & m_{13} m'_{12} &
//          m_{11} m'_{13} & m_{12} m'_{13}  & m_{13} m'_{13}
//       \\ \vdots & & & & & & & &
//       \\ m_{n1} m'_{n1} & m_{n2} m'_{n1}  & m_{n3} m'_{n1} &
//          m_{n1} m'_{n2} & m_{n2} m'_{n2}  & m_{n3} m'_{n2} &
//          m_{n1} m'_{n3} & m_{n2} m'_{n3}  & m_{n3} m'_{n3}
//     \end{array} \right)
// \f]
// such that $ D f = 0 $ for
// $f = (F_{11} F_{12} F_{13} F_{21} F_{22} F_{23} F_{31} F_{32} F_{33}) $
// where $F$ is the fundamental matrix.
// }
//

FDesignMatrix::FDesignMatrix(vcl_vector<HomgPoint2D>& points1,
                             vcl_vector<HomgPoint2D>& points2):
  base(points1.size(), 9)
{
  int n = points1.size();
  for(int row = 0; row < n; ++row) {
    const HomgPoint2D& p1 = points1[row];
    const HomgPoint2D& p2 = points2[row];

    vnl_matrix<double>& D = *this;
    D(row,0) = p1.get_x() * p2.get_x();
    D(row,1) = p1.get_y() * p2.get_x();
    D(row,2) = p1.get_w() * p2.get_x();
    D(row,3) = p1.get_x() * p2.get_y();
    D(row,4) = p1.get_y() * p2.get_y();
    D(row,5) = p1.get_w() * p2.get_y();
    D(row,6) = p1.get_x() * p2.get_w();
    D(row,7) = p1.get_y() * p2.get_w();
    D(row,8) = p1.get_w() * p2.get_w();
  }
}
