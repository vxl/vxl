#ifndef bugl_normal_point_3d_h_
#define bugl_normal_point_3d_h_
//:
// \file
// \brief a double type specification of bugl_gaussian_point_3d
// \author Kongbin Kang
//
// \verbatim
//  Modifications
//   10 Sept 2004 Peter Vanroose - pass arguments as "const&" instead of as "&"
// \endverbatim

#include <bugl/bugl_gaussian_point_3d.h>
#include <vbl/vbl_ref_count.h>

class bugl_normal_point_3d : public bugl_gaussian_point_3d<double>, public vbl_ref_count
{
 public:
  bugl_normal_point_3d() {}

  bugl_normal_point_3d(vgl_point_3d<double> &p, vnl_matrix_fixed<double,3,3> &s)
    : bugl_gaussian_point_3d<double>(p,s), vbl_ref_count() {}

  bugl_normal_point_3d(double vx, double vy, double vz, vnl_matrix_fixed<double,3,3> &s)
    : bugl_gaussian_point_3d<double>(vx,vy,vz,s), vbl_ref_count() {}

  bugl_normal_point_3d(bugl_normal_point_3d const& p)
    : bugl_gaussian_point_3d<double>(p), vbl_ref_count() {}

  ~bugl_normal_point_3d() {}
};

#endif
