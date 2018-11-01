// This is oxl/mvl/FDesignMatrix.h
#ifndef FDesignMatrix_h_
#define FDesignMatrix_h_
//:
// \file
// \brief Design matrix for linear FMatrix least squares
//
//    FDesignMatrix is a subclass of Matrix that, given $n$ pairs of homogeneous
//    2D points, creates the $n \times 9$ design matrix for the linear least squares
//    problem $(m_2^\top F m_1)^2 = 0$ where $F$ is the fundamental matrix.
//
//    As the design matrix is common to a number of algorithms, this reduces
//    code duplication.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 Aug 96
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/HomgPoint2D.h>
#include <vgl/vgl_homg_point_2d.h>

class FDesignMatrix : public vnl_matrix<double>
{
  typedef vnl_matrix<double> base;
 public:
  // Constructors/Destructors--------------------------------------------------

  FDesignMatrix(std::vector<HomgPoint2D> const& points1,
                std::vector<HomgPoint2D> const& points2);

  FDesignMatrix(std::vector<vgl_homg_point_2d<double> > const& points1,
                std::vector<vgl_homg_point_2d<double> > const& points2);
};

#endif // FDesignMatrix_h_
