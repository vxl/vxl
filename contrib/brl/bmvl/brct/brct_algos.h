//:
// \file
// \brief collection class for various reconstruction functions
// \author Kongbin Kang (Kongbin_Kang@Brown.edu)
// \date   4/24/2003
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#ifndef bcrt_algos_h_
#define bcrt_algos_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vsol/vsol_box_3d_sptr.h>

class brct_algos
{
 public:
  brct_algos();
  virtual ~brct_algos();

  //operators
 public:
  static void add_box_vrml(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);
  static vsol_box_3d_sptr get_bounding_box(vcl_vector<vgl_point_3d<double> > &pts_3d);
  static vnl_double_3 bundle_reconstruct_3d_point(vcl_vector<vnl_double_2> &pts,
                                                  vcl_vector<vnl_double_3x4>  &P);
  static vgl_point_2d<double> projection_3d_point(const vgl_point_3d<double> & x, const vnl_double_3x4& P);
  //: pointwise reconstruction
  static vgl_point_3d<double> triangulate_3d_point(const vgl_point_2d<double>& x1, const vnl_double_3x4& P1,
                                                   const vgl_point_2d<double>& x2, const vnl_double_3x4& P2);
};

#endif // bcrt_algos_h_
