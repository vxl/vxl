// This is core/vgl/algo/vgl_homg_operators_1d.h
#ifndef vgl_homg_operators_1d_h_
#define vgl_homg_operators_1d_h_
//:
// \file
// \brief 1D homogeneous functions
//
// vgl_homg_operators_1d implements one-dimensional homogeneous functions.
//
// \author
//     Peter Vanroose, ESAT/PSI, Nov. 1998.
//-----------------------------------------------------------------------------
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

#include <vgl/vgl_homg_point_1d.h>
#include <vnl/vnl_fwd.h>

//: 1D homogeneous functions
template <class T>
class vgl_homg_operators_1d
{
 public:
  static double cross_ratio(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b,
                            const vgl_homg_point_1d<T>& c, const vgl_homg_point_1d<T>& d);
  static T conjugate(T x1, T x2, T x3, double cr = -1);
  static vgl_homg_point_1d<T> conjugate(const vgl_homg_point_1d<T>& a,
                                        const vgl_homg_point_1d<T>& b,
                                        const vgl_homg_point_1d<T>& c,
                                        double cr = -1);

  static T dot(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b);
  static T cross(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b);
  static void unitize(vgl_homg_point_1d<T>& a);

  static T distance(const vgl_homg_point_1d<T>& point1, const vgl_homg_point_1d<T>& point2);
  static T distance_squared(const vgl_homg_point_1d<T>& point1, const vgl_homg_point_1d<T>& point2);

  static bool is_within_distance(const vgl_homg_point_1d<T>& p1, const vgl_homg_point_1d<T>& p2, T d)
  {
    return distance(p1, p2) < d;
  }
  static vgl_homg_point_1d<T> midpoint(const vgl_homg_point_1d<T>& p1, const vgl_homg_point_1d<T>& p2);
};

//: Transform a point through a 2x2 projective transformation matrix
// \relates vgl_homg_point_1d
template <class T>
vgl_homg_point_1d<T> operator*(vnl_matrix_fixed<T,2,2> const& m,
                               vgl_homg_point_1d<T> const& p);

#define VGL_HOMG_OPERATORS_1D_INSTANTIATE(T) \
        "Please #include <vgl/algo/vgl_homg_operators_1d.txx>"

#endif // vgl_homg_operators_1d_h_
