#ifndef bvgl_h_matrix_1d_compute_3point_h_
#define bvgl_h_matrix_1d_compute_3point_h_
//:
// \file
//
// bvgl_h_matrix_1d_compute_linear contains a method to calculate
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

#include <bvgl/bvgl_h_matrix_1d_compute.h>

class bvgl_h_matrix_1d_compute_3point : public bvgl_h_matrix_1d_compute
{
 public:
  bvgl_h_matrix_1d_compute_3point(void);
  ~bvgl_h_matrix_1d_compute_3point();
 protected:
  bool 
    compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > & points1,
                      const vcl_vector<vgl_homg_point_1d<double> > & points2,
                      bvgl_h_matrix_1d<double>& H);
};

#endif // bvgl_h_matrix_1d_compute_3point_h_
