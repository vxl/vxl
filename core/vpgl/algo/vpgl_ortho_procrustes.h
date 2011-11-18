// This is core/vpgl/algo/vpgl_ortho_procrustes.h
#ifndef vpgl_ortho_procrustes_h_
#define vpgl_ortho_procrustes_h_
//:
// \file
// \brief Solve min(R,s) ||X-s(RY+t)||, where R is a rotation matrix, X,Y are 3-d points, s is a scalar and t is a translation vector.
// \author J. L. Mundy
// \date June 29, 2007
//

#include <vnl/vnl_fwd.h>
#include <vgl/algo/vgl_rotation_3d.h>

//: Solve orthogonal Procrustes problem
// Solve the orthogonal Procrustes problem by finding a rotation matrix, R,
// scale factor, s, and translation vector, t, that minimizes the distance
// between two pointsets, X and Y, where Y is transformed to produce X.
class vpgl_ortho_procrustes
{
 public:
  //: only one constructor X and Y must both have dimensions 3 x N
  vpgl_ortho_procrustes(vnl_matrix<double> const& X,
                        vnl_matrix<double> const& Y);

  //: the resulting rotation matrix
  vgl_rotation_3d<double> R();

  //: the resulting translation vector
  vnl_vector_fixed<double, 3> t();

  //: The scale factor, s
  double s();

  //: the residual error
  double residual_mean_sq_error();

  //: successful computation
  bool compute_ok() const { return !cannot_compute_; }

 protected:
  //: No default constructor
  vpgl_ortho_procrustes();
  void compute();

  //: members
  bool cannot_compute_;
  bool computed_;
  vnl_matrix<double> X_;
  vnl_matrix<double> Y_;
  vgl_rotation_3d<double> R_;
  vnl_vector_fixed<double, 3> t_;
  double s_;
  double residual_;
};

#endif // vpgl_ortho_procrustes_h_
