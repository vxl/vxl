// This is gel/mrc/vpgl/algo/vpgl_ortho_procrustes.h
#ifndef vpgl_ortho_procrustes_h_
#define vpgl_ortho_procrustes_h_
//:
// \file
// \brief Solve min(R,s) ||X-sRY||, where R is a rotation matrix, X,Y are 3-d points, s is a scalar.
// \author J. L. Mundy
// \date June 29, 2007
//

#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vgl/algo/vgl_rotation_3d.h>
//: Solve the ortogonal Procrustes problem by finding a rotation matrix, R,
// and scale factor, s, that minimizes the distance between two pointsets,
// X and Y, where Y is transformed by R to produce X
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
  bool compute_ok(){return !cannot_compute_;}

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
