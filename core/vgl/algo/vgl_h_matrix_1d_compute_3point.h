#ifndef vgl_h_matrix_1d_compute_3point_h_
#define vgl_h_matrix_1d_compute_3point_h_
//:
// \file
//
// vgl_h_matrix_1d_compute_linear contains a method to calculate
// the line projectivity which relates three 1D point correspondences.
//
// \author
// F. Schaffalitzky, RRG
//
// \verbatim
// Modifications
//    23 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl 
//                  computations restricted to vgl_homg_point_1d<double>
//                  Seems somewhat overdoing it to template the transform
//                  solvers since double is needed for robust computation 
// \endverbatim

#include <vgl/algo/vgl_h_matrix_1d_compute.h>

class vgl_h_matrix_1d_compute_3point : public vgl_h_matrix_1d_compute
{
 public:
  vgl_h_matrix_1d_compute_3point(void);
  ~vgl_h_matrix_1d_compute_3point();
 protected:
  bool 
    compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > & points1,
                      const vcl_vector<vgl_homg_point_1d<double> > & points2,
                      vgl_h_matrix_1d<double>& H);
};

#endif // vgl_h_matrix_1d_compute_3point_h_
