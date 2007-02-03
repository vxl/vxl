// This is core/vgl/algo/vgl_homg_operators_1d.h
#ifndef vgl_homg_operators_1d_h_
#define vgl_homg_operators_1d_h_
//:
// \file
// \brief  1D homogeneous functions
// \author Peter Vanroose, ESAT/PSI
// \date   Nov. 1998
//
// vgl_homg_operators_1d implements one-dimensional homogeneous functions.
//
// \verbatim
// Modifications
//    3-Feb-07 Peter Vanroose - added get_vector()
// \endverbatim
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
  //: get a vnl_vector_fixed representation of a homogeneous object
  static vnl_vector_fixed<T,2> get_vector(vgl_homg_point_1d<T> const& p);

  //: cross ratio of four 1D points
  // This number is projectively invariant, and it is the coordinate of p4
  // in the reference frame where p2 is the origin (coordinate 0), p3 is
  // the unity (coordinate 1) and p1 is the point at infinity.
  // This cross ratio is often denoted as ((p1, p2; p3, p4)) (which also
  // equals ((p3, p4; p1, p2)) or ((p2, p1; p4, p3)) or ((p4, p3; p2, p1)) )
  // and is calculated as
  //  \verbatim
  //                      p1 - p3   p2 - p3      (p1-p3)(p2-p4)
  //                      ------- : --------  =  --------------
  //                      p1 - p4   p2 - p4      (p1-p4)(p2-p3)
  // \endverbatim
  // where pi are nonhomogeneous coordinates for the four points.
  //
  static double cross_ratio(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b,
                            const vgl_homg_point_1d<T>& c, const vgl_homg_point_1d<T>& d);

  //: Calculate the projective conjugate point of three given points.
  // Or more generally, the point with a given crossratio w.r.t. three other points:
  // The cross ratio ((x1,x2;x3,answer)) is cr (default -1). When cr is -1,
  // the returned value and x3 are conjugate points w.r.t. the pair (x1,x2).
  // Because this function is transitive on coordinates, it is sufficient to
  // implement it for 1-dimensional points, i.e., for scalars.
  static T conjugate(T x1, T x2, T x3, double cr = -1);

  //: Calculate the projective conjugate point of three given points.
  // Or more generally, the point with a given crossratio w.r.t. three other points:
  // The cross ratio ((x1,x2;x3,answer)) is cr (default -1). When cr is -1,
  // the returned value and x3 are conjugate points w.r.t. the pair (x1,x2).
  static vgl_homg_point_1d<T> conjugate(const vgl_homg_point_1d<T>& a,
                                        const vgl_homg_point_1d<T>& b,
                                        const vgl_homg_point_1d<T>& c,
                                        double cr = -1);

  //: Dot product of two homogeneous points
  static T dot(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b);

  //: Cross product of two homogeneous points
  static T cross(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b);

  //: Normalize vgl_homg_point_1d<T> to unit magnitude
  static void unitize(vgl_homg_point_1d<T>& a);

  //: Get the distance between the two points.
  static T distance(const vgl_homg_point_1d<T>& point1, const vgl_homg_point_1d<T>& point2);

  //: Get the square of the distance between the two points.
  static T distance_squared(const vgl_homg_point_1d<T>& point1, const vgl_homg_point_1d<T>& point2);

  //: True if the points are closer than Euclidean distance d.
  static bool is_within_distance(const vgl_homg_point_1d<T>& p1, const vgl_homg_point_1d<T>& p2, T d)
  {
    return distance(p1, p2) < d;
  }

  //: Return the midpoint of two homogeneous points
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
