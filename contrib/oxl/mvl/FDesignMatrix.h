#ifndef FDesignMatrix_h_
#define FDesignMatrix_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME    FDesignMatrix - Design matrix for linear FMatrix least squares
// .LIBRARY MViewCompute
// .HEADER  MultiView Package
// .INCLUDE mvl/FDesignMatrix.h
// .FILE    FDesignMatrix.cxx
// .SECTION Description
//    @{ FDesignMatrix is a subclass of Matrix that, given $n$ pairs of homogeneous
//    2D points, creates the $n \times 9$ design matrix for the linear least squares
//    problem $(m_2^\top F m_1)^2 = 0$ where $F$ is the fundamental matrix. @}
//
//    As the design matrix is common to a number of algorithms, this reduces
//    code duplication.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <mvl/HomgPoint2D.h>

class FDesignMatrix : public vnl_matrix<double> {
  typedef vnl_matrix<double> base;
public:
  // Constructors/Destructors--------------------------------------------------

  FDesignMatrix(vcl_vector<HomgPoint2D> const& points1,
                vcl_vector<HomgPoint2D> const& points2);
};

#endif // FDesignMatrix_h_
