// This is oxl/mvl/FDesignMatrix.cxx
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

FDesignMatrix::FDesignMatrix(std::vector<HomgPoint2D> const& points1,
                             std::vector<HomgPoint2D> const& points2):
  base(points1.size(), 9)
{
  unsigned int n = points1.size();
  for (unsigned int row = 0; row < n; ++row) {
    const HomgPoint2D& p1 = points1[row];
    const HomgPoint2D& p2 = points2[row];

    vnl_matrix<double>& D = *this;
    D(row,0) = p1.x() * p2.x();
    D(row,1) = p1.y() * p2.x();
    D(row,2) = p1.w() * p2.x();
    D(row,3) = p1.x() * p2.y();
    D(row,4) = p1.y() * p2.y();
    D(row,5) = p1.w() * p2.y();
    D(row,6) = p1.x() * p2.w();
    D(row,7) = p1.y() * p2.w();
    D(row,8) = p1.w() * p2.w();
  }
}

FDesignMatrix::FDesignMatrix(std::vector<vgl_homg_point_2d<double> > const& points1,
                             std::vector<vgl_homg_point_2d<double> > const& points2):
  base(points1.size(), 9)
{
  unsigned int n = points1.size();
  for (unsigned int row = 0; row < n; ++row) {
    const vgl_homg_point_2d<double>& p1 = points1[row];
    const vgl_homg_point_2d<double>& p2 = points2[row];

    vnl_matrix<double>& D = *this;
    D(row,0) = p1.x() * p2.x();
    D(row,1) = p1.y() * p2.x();
    D(row,2) = p1.w() * p2.x();
    D(row,3) = p1.x() * p2.y();
    D(row,4) = p1.y() * p2.y();
    D(row,5) = p1.w() * p2.y();
    D(row,6) = p1.x() * p2.w();
    D(row,7) = p1.y() * p2.w();
    D(row,8) = p1.w() * p2.w();
  }
}
