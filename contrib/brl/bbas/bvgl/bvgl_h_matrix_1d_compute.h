// This is brl/bbas/bvgl/bvgl_h_matrix_1d_compute.h
#ifndef bvgl_h_matrix_1d_compute_h_
#define bvgl_h_matrix_1d_compute_h_
//:
// \file
//
// Base class of classes to generate a line-to-line projectivity matrix from
// a set of matched points.
//
// \author
//  Frederik Schaffalitzky , Robotic Research Group
//
// \verbatim
// Modifications
//    23 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl 
//                  computations restricted to vgl_homg_point_1d<double>
//                  Seems somewhat overdoing it to template the transform
//                  solvers since double is needed for robust computation 
// \endverbatim

#include <vgl/vgl_homg_point_1d.h>
#include "bvgl_h_matrix_1d.h"
#include <vcl_vector.h>

class bvgl_h_matrix_1d_compute
{
 public:
  //
  bvgl_h_matrix_1d_compute() : verbose_(false) { }
  virtual ~bvgl_h_matrix_1d_compute() { }

  //
  void verbose(bool); // set this to true for verbose run-time information

  //
  // Compute methods :
  //
  bool compute(const vcl_vector<vgl_homg_point_1d<double> >&,
               const vcl_vector<vgl_homg_point_1d<double> >&,
               bvgl_h_matrix_1d<double>& H );

  bool compute(const double [],
               const double [],
               int,
               bvgl_h_matrix_1d<double> *);
 protected:
  bool verbose_;

  virtual bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > &,
                                 const vcl_vector<vgl_homg_point_1d<double> > &,
                                 bvgl_h_matrix_1d<double>& H);

  virtual bool compute_array_dbl(const double [],
                                 const double [],
                                 int N,
                                 bvgl_h_matrix_1d<double>& H);
};

#endif // bvgl_h_matrix_1d_compute_h_
