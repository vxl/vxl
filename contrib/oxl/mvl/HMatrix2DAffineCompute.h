// This is oxl/mvl/HMatrix2DAffineCompute.h
#ifndef HMatrix2DAffineCompute_h_
#define HMatrix2DAffineCompute_h_
//:
// \file
//
// HMatrix2DAffineCompute contains a linear method to compute
// a 2D affine transformation. The H returned is such that
// \f\[ x_2 \sim H x_1 \f\]
//
// \author   David Capel, Oxford RRG, 13 May 98
// \verbatim
//  Modifications:
//     FSM 23-08-98 Added constructor so that the class can be used
//                  as a compute object. Removed compute() method
//                  taking PairMatchSet argument. Changed the remaining
//                  compute method to take an HMatrix2D* argument instead
//                  of returning an HMatrix2D.
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim

#include <mvl/HMatrix2DCompute.h>
#include <mvl/HMatrix2D.h>
#include <vgl/vgl_homg_point_2d.h>
class HomgPoint2D;

class HMatrix2DAffineCompute : public HMatrix2DCompute
{
 protected:
  bool compute_p(std::vector<HomgPoint2D> const&,
                 std::vector<HomgPoint2D> const&,
                 HMatrix2D *) override;
 public:
  static HMatrix2D compute(const std::vector<HomgPoint2D>&p1, const std::vector<HomgPoint2D>&p2);
  static HMatrix2D compute(std::vector<vgl_homg_point_2d<double> > const& p1,
                           std::vector<vgl_homg_point_2d<double> > const& p2);
  int minimum_number_of_correspondences() const override { return 3; }
  // left in for capes :
  static HMatrix2D compute(const PairMatchSetCorner &matches);
};

//--------------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
//
// This class takes an array of n HomgPoint2Ds and creates
// an n-by-2 matrix whose ith row contains the inhomogeneous
// coordinates of the ith homogeneous point.
//
struct NonHomg : public vnl_matrix<double>
{
  NonHomg(const std::vector<HomgPoint2D> &A);
  NonHomg(std::vector<vgl_homg_point_2d<double> > const& A);
};

//
// This function computes the mean of the columns of
// an n-by-2 matrix A.
//
vnl_double_2 mean2(const vnl_matrix<double> &A);


//
// This function subtracts the 2-vector a from each row of
// the n-by-2 matrix A.
//
vnl_matrix<double>& sub_rows(vnl_matrix<double> &A, const vnl_double_2 a);

//--------------------------------------------------------------------------------

#endif // HMatrix2DAffineCompute_h_
