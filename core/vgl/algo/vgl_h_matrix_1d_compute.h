#ifndef vgl_h_matrix_1d_compute_h_
#define vgl_h_matrix_1d_compute_h_
//:
// \file
// \brief Virtual base class of classes to generate a line-to-line projectivity from a set of matched points
//
// \author
//  Frederik Schaffalitzky , Robotic Research Group
//
// \verbatim
//  Modifications
//   23 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//                 computations restricted to vgl_homg_point_1d<double>
//                 Seems somewhat overdoing it to template the transform
//                 solvers since double is needed for robust computation
//   23 Jun 2003 - Peter Vanroose - made compute_cool_homg pure virtual
//   24 Jun 2003 - Peter Vanroose - implemented the second compute() method
//   13 Jun 2004 - Peter Vanroose - added compute() overload, similar to 2d interface
// \endverbatim

#include <vgl/vgl_homg_point_1d.h>
#include <vgl/algo/vgl_h_matrix_1d.h>
#include <vcl_vector.h>

class vgl_h_matrix_1d_compute
{
 public:
  //
  vgl_h_matrix_1d_compute() : verbose_(false) {}
  virtual ~vgl_h_matrix_1d_compute() {}

  //: set this to true for verbose run-time information; default is false
  void verbose(bool v) { verbose_=v; }

  //
  // Compute methods :
  //

  //: principal interface: given point correspondences in p1,p2, returns H
  bool compute(const vcl_vector<vgl_homg_point_1d<double> >& p1,
               const vcl_vector<vgl_homg_point_1d<double> >& p2,
               vgl_h_matrix_1d<double>& H)
  { return compute_cool_homg(p1,p2,H); } // calls pure virtual function

  //: nonhomogeneous interface: given point correspondences in p1,p2, returns H
  bool compute(const double p1[],
               const double p2[],
               unsigned int length, // length of both p1 and p2
               vgl_h_matrix_1d<double>& H)
  { return compute_array_dbl(p1,p2,length,H); }

  //: homography from matched points - return h_matrix
  vgl_h_matrix_1d<double>
  compute(vcl_vector<vgl_homg_point_1d<double> > const& p1,
          vcl_vector<vgl_homg_point_1d<double> > const& p2)
  { vgl_h_matrix_1d<double> H; compute(p1, p2, H); return H; }

 protected:
  bool verbose_;

  virtual bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > &,
                                 const vcl_vector<vgl_homg_point_1d<double> > &,
                                 vgl_h_matrix_1d<double>& H) = 0;

  bool compute_array_dbl(const double p1[], const double p2[], unsigned int length,
                         vgl_h_matrix_1d<double>& H);
};

#endif // vgl_h_matrix_1d_compute_h_
