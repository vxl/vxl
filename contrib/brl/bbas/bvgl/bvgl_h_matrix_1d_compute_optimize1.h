#ifndef _bvgl_h_matrix_1d_compute_optimize1_h_
#define _bvgl_h_matrix_1d_compute_optimize1_h_
//:
// \file
//
// bvgl_h_matrix_1d_compute_optimize1 compute the h_matrix using Levenberg-Marquard.
//
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

class bvgl_h_matrix_1d_compute_optimize1 : public bvgl_h_matrix_1d_compute
{
 protected:
  bool 
    compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >& points1,
                      const vcl_vector<vgl_homg_point_1d<double> >& points2,
                         bvgl_h_matrix_1d<double>& H);
 public:
  bvgl_h_matrix_1d_compute_optimize1(void);
  ~bvgl_h_matrix_1d_compute_optimize1();
};

typedef bvgl_h_matrix_1d_compute_optimize1 bvgl_h_matrix_1d_computeOptimise1;

#endif // _bvgl_h_matrix_1d_compute_optimize1_h_
