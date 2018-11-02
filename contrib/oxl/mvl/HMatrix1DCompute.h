#ifndef HMatrix1DCompute_h_
#define HMatrix1DCompute_h_
//:
// \file
//
// Base class of classes to generate a line-to-line projectivity matrix from
// a set of matched points.
//
// \author fsm
//

class HMatrix1D;
#include <iostream>
#include <vector>
#include <vgl/vgl_homg_point_1d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HMatrix1DCompute
{
 public:
  //
  HMatrix1DCompute() : verbose_(false) { }
  virtual ~HMatrix1DCompute() = default;

  //
  void verbose(bool); // set this to true for verbose run-time information

  //
  // Compute methods :
  //
  bool compute(const std::vector<vgl_homg_point_1d<double> >&,
               const std::vector<vgl_homg_point_1d<double> >&,
               HMatrix1D *);
  bool compute(const double [],
               const double [],
               int,
               HMatrix1D *);
 protected:
  bool verbose_;

  virtual bool compute_cool_homg(const std::vector<vgl_homg_point_1d<double> >&,
                                 const std::vector<vgl_homg_point_1d<double> >&,
                                 HMatrix1D *);
  virtual bool compute_array_dbl(const double [],
                                 const double [],
                                 int N,
                                 HMatrix1D *);
};

#endif // HMatrix1DCompute_h_
