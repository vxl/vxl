#ifndef bugl_normal_point_2d_h_
#define bugl_normal_point_2d_h_
//:
// \file
// \brief a double type specification of bugl_gaussian_point_2d
// \author Kongbin Kang
//
// \verbatim
//  Modifications
//   10 Sept 2004 Peter Vanroose - pass arguments as "const&" instead of as "&"
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <bugl/bugl_gaussian_point_2d.h>

//: a specification of template class bugl_gaussian_point_2d
//
class bugl_normal_point_2d: public bugl_gaussian_point_2d<double>, public vbl_ref_count
{
 public:
  bugl_normal_point_2d(vgl_point_2d<double> &p, vnl_matrix_fixed<double,2,2> &s)
    : bugl_gaussian_point_2d<double>(p,s), vbl_ref_count() {}

  bugl_normal_point_2d(double x, double y, vnl_matrix_fixed<double,2,2> &s)
    : bugl_gaussian_point_2d<double>(x,y,s), vbl_ref_count() {}

  bugl_normal_point_2d(bugl_normal_point_2d const& p)
    : bugl_gaussian_point_2d<double>(p), vbl_ref_count() {}

  bugl_normal_point_2d() {}

  ~bugl_normal_point_2d() {}
};

#endif // bugl_normal_point_2d_h_
