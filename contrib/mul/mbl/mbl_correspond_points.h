#ifndef mbl_correspond_points_h_
#define mbl_correspond_points_h_
//:
// \file
// \brief Shapiro & Brady's point correspondence algorithm
// \author Tim Cootes

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_point_2d.h>

//: Shapiro & Brady's point correspondence algorithm.
//  Includes improvements by Carcassoni and Hancock (a robust kernel
//  in the distance metric).
//
//  Note that current version can't cope with different numbers of points.
//  Need to re-order eigenvectors by absolute magnitude of eigenvalues.
//  I'll do this soon - Tim.
class mbl_correspond_points
{
 private:
  //: Eigenvalues of proximity matrix 1
  vnl_vector<double> evals1_;
  //: Eigenvalues of proximity matrix 2
  vnl_vector<double> evals2_;

  //: Return index of row in H2 most similar to row i of H1
  unsigned closest_row(const vnl_matrix<double>& H1,
                       const vnl_matrix<double>& H2,
                       unsigned i);

  //: Ensure each column vector points in the same way
  //  Unit eigenvectors can point in one of two directions.
  //  Ensure p.1>=0 to obtain consistancy.
  void fix_eigenvectors(vnl_matrix<double>& P);

 public:

  //: Dflt ctor
  mbl_correspond_points();

  //: Find best correspondence between points1 and points2
  //  On exit, matches[i] gives index of points2 which
  //  corresponds to points1[i].
  //  Note that there may be a many to one correspondence produced.
  //  \param sigma Scaling factor defining kernel width
  void correspond(const std::vector<vgl_point_2d<double> >& points1,
                  const std::vector<vgl_point_2d<double> >& points2,
                  std::vector<unsigned>& matches, double sigma);

  //: Construct proximity matrix using cosh kernel
  //  On exit, D(i,j) = tanh(pi*d_ij/sigma) * 2/(pi*d_ij)
  //  where d_ij is the distance between points i and j
  void proximity_by_tanh(const std::vector<vgl_point_2d<double> >& points,
                         vnl_matrix<double>& H, double sigma);

  //: Eigenvalues of proximity matrix 1 in last call to correspond
  const vnl_vector<double>& evals1() const { return evals1_; }

  //: Eigenvalues of proximity matrix 2 in last call to correspond
  const vnl_vector<double>& evals2() const { return evals2_; }
};

#endif //mbl_correspond_points_h_
