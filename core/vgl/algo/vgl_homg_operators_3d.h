// This is core/vgl/algo/vgl_homg_operators_3d.h
#ifndef vgl_homg_operators_3d_h_
#define vgl_homg_operators_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Don Hamilton, Peter Tu
// \date Feb 16 2000
//
// \verbatim
//  Modifications
//   31-oct-00 Peter Vanroose - implementations fixed, and vgl_homg_line_3d typedef'd
//   16-Mar-01 Tim Cootes - Tidied up documentation
// \endverbatim

#include <vcl_vector.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_homg_line_3d_2_points.h>

template <class Type> class vgl_homg_point_3d;
template <class Type> class vgl_homg_line_3d_2_points;
template <class Type> class vgl_homg_plane_3d;

//: 3D homogeneous operations
template <class Type>
class vgl_homg_operators_3d
{
  typedef vgl_homg_line_3d_2_points<Type> vgl_homg_line_3d;

 public:

  // method to get a vnl_vector rep of a homogeneous object

  //: Get vnl_vector rep of a homogeneous object
  static vnl_vector<Type> get_vector(vgl_homg_point_3d<Type> const& p);
  //: Get vnl_vector rep of a homogeneous object
  static vnl_vector<Type> get_vector(vgl_homg_plane_3d<Type> const& p);

  static double angle_between_oriented_lines(const vgl_homg_line_3d& line1,
                                             const vgl_homg_line_3d& line2);
  static Type distance(const vgl_homg_point_3d<Type>& point1,
                       const vgl_homg_point_3d<Type>& point2);
  static Type distance_squared(const vgl_homg_point_3d<Type>& point1,
                               const vgl_homg_point_3d<Type>& point2);
  static vgl_homg_point_3d<Type> intersect_line_and_plane(const vgl_homg_line_3d&,
                                                          const vgl_homg_plane_3d<Type>&);
#if 0 // not yet implemented
  static vgl_homg_point_3d<Type> lines_to_point(const vgl_homg_line_3d& line1,
                                                const vgl_homg_line_3d& line2);
  static vgl_homg_point_3d<Type> lines_to_point(const vcl_vector<vgl_homg_line_3d>& line_list);
#endif // 0
  static double perp_distance_squared(const vgl_homg_line_3d& line,
                                      const vgl_homg_point_3d<Type>& point);
  static vgl_homg_line_3d perp_line_through_point(const vgl_homg_line_3d& line,
                                                  const vgl_homg_point_3d<Type>& point);
  static vgl_homg_point_3d<Type> perp_projection(const vgl_homg_line_3d& line,
                                                 const vgl_homg_point_3d<Type>& point);
  static vgl_homg_line_3d planes_to_line(const vgl_homg_plane_3d<Type>& plane1,
                                         const vgl_homg_plane_3d<Type>& plane2);
#if 0 // not yet implemented
  static vgl_homg_line_3d planes_to_line(const vcl_vector<vgl_homg_plane_3d<Type> >& plane_list);
  static vgl_homg_line_3d points_to_line(const vgl_homg_point_3d<Type>& point1,
                                         const vgl_homg_point_3d<Type>& point2);
  static vgl_homg_line_3d points_to_line(const vcl_vector<vgl_homg_point_3d<Type> >& point_list);

  static vgl_homg_plane_3d<Type> points_to_plane(const vgl_homg_point_3d<Type>&,
                                                 const vgl_homg_point_3d<Type>&,
                                                 const vgl_homg_point_3d<Type>& );
  static vgl_homg_plane_3d<Type> points_to_plane(const vcl_vector<vgl_homg_point_3d<Type> >& point_list);
#endif // 0
  static vgl_homg_point_3d<Type> intersection_point(const vgl_homg_plane_3d<Type>&,
                                                    const vgl_homg_plane_3d<Type>&,
                                                    const vgl_homg_plane_3d<Type>&);
  static vgl_homg_point_3d<Type> intersection_point(const vcl_vector<vgl_homg_plane_3d<Type> >&);

  //-----------------------------------------------------------------------------
  //: Calculates the cross ratio of four collinear points p1, p2, p3 and p4.
  // This number is projectively invariant, and it is the coordinate of p4
  // in the reference frame where p2 is the origin (coordinate 0), p3 is
  // the unity (coordinate 1) and p1 is the point at infinity.
  // This cross ratio is often denoted as ((p1, p2; p3, p4)) (which also
  // equals ((p3, p4; p1, p2)) or ((p2, p1; p4, p3)) or ((p4, p3; p2, p1)) )
  // and is calculated as
  // \verbatim
  //                      p1 - p3   p2 - p3      (p1-p3)(p2-p4)
  //                      ------- : --------  =  --------------
  //                      p1 - p4   p2 - p4      (p1-p4)(p2-p3)
  // \endverbatim
  //
  // In principle, any single nonhomogeneous coordinate from the four points
  // can be used as parameters for cross_ratio (but of course the same for all
  // points). The most reliable answer will be obtained when the coordinate with
  // the largest spacing is used, i.e., the one with smallest slope.
  //
  // In this implementation, a least-squares result is calculated when the
  // points are not exactly collinear.

  static double cross_ratio(const vgl_homg_point_3d<Type >& p1,
                            const vgl_homg_point_3d<Type >& p2,
                            const vgl_homg_point_3d<Type >& p3,
                            const vgl_homg_point_3d<Type >& p4);
  static double cross_ratio(const vgl_homg_plane_3d<Type >& p1,
                            const vgl_homg_plane_3d<Type >& p2,
                            const vgl_homg_plane_3d<Type >& p3,
                            const vgl_homg_plane_3d<Type >& p4);
};

template <class T>
vgl_homg_point_3d<T> operator*(vnl_matrix_fixed<T,4,4> const& m,
                               vgl_homg_point_3d<T> const& p);

//: Project a 3D point to 2D through a 3x4 projective transformation matrix
template <class T>
vgl_homg_point_2d<T> operator*(vnl_matrix_fixed<T,3,4> const& m,
                               vgl_homg_point_3d<T> const& p);

#if 0 // does not make sense -- fsm
//: Project a plane to a 2D line through a 3x4 projective transformation matrix
template <class T>
vgl_homg_line_2d<T> operator*(vnl_matrix_fixed<T,3,4> const& m,
                              vgl_homg_plane_3d<T> const& l);
#endif // 0

//: Backproject a 2D line through a 4x3 projective transformation matrix
template <class T>
vgl_homg_plane_3d<T> operator*(vnl_matrix_fixed<T,4,3> const& m,
                               vgl_homg_line_2d<T> const& l);

#define VGL_HOMG_OPERATORS_3D_INSTANTIATE(T) \
        "Please #include <vgl/algo/vgl_homg_operators_3d.txx>"

#endif // vgl_homg_operators_3d_h_
