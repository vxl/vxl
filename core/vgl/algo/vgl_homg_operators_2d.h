#ifndef vgl_homg_operations_2d_h
#define vgl_homg_operations_2d_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Don Hamilton, Peter Tu
// Created: Feb 16 2000
// Modifications
//   31-oct-00 Peter Vanroose - signatures fixed, and vcl_vector iterator used
//   16-Mar-01 Tim Cootes - added documentation

#include <vnl/vnl_vector.h>
template <class Type> class vgl_homg_point_2d;
template <class Type> class vgl_homg_line_2d;
template <class Type> class vgl_homg_point_2d;
#include <vcl_vector.h>

//: 2D homogeneous operations
template <class Type>
class vgl_homg_operators_2d {
public:
  //: get a vnl_vector representation of a homogeneous object
  static vnl_vector<Type> get_vector(vgl_homg_point_2d<Type> const& p);

  //: get a vnl_vector representation of a homogeneous object
  static vnl_vector<Type> get_vector(vgl_homg_line_2d<Type> const& l);

  //: Normalize vgl_homg_point_2d<Type> to unit magnitude
  static void unitize(vgl_homg_point_2d<Type>& a);

  static double angle_between_oriented_lines (const vgl_homg_line_2d<Type>& line1,
                                              const vgl_homg_line_2d<Type>& line2);
  //: Get the 0 to pi/2 angle between two lines
  static double abs_angle (const vgl_homg_line_2d<Type>& line1,
                           const vgl_homg_line_2d<Type>& line2);

  //: Get the square of the 2D distance between the two points.
  static double distance_squared (const vgl_homg_point_2d<Type>& point1, const vgl_homg_point_2d<Type>& point2);
  static double perp_distance_squared (const vgl_homg_line_2d<Type>& line,
                                       const vgl_homg_point_2d<Type>& point);

  //: Get the square of the perpendicular distance to a line.
  // This is just the homogeneous form of the familiar
  // \f$ \frac{a x + b y + c}{\sqrt{a^2+b^2}} \f$ :
  // \[ d = \frac{(l^\top p)}{p_z\sqrt{l_x^2 + l_y^2}} \]
  // If either the point or the line are at infinity an error message is
  // printed and Homg::infinity is returned.
  static double perp_dist_squared (const vgl_homg_point_2d<Type>& point,
                                   const vgl_homg_line_2d<Type>& line);

  //: True if the points are closer than euclidean distance d.
  static bool is_within_distance(const vgl_homg_point_2d<Type>& p1,
                                 const vgl_homg_point_2d<Type>& p2, double d)
  {
    return distance_squared(p1, p2) < d*d;
  }

  //: Get the anticlockwise angle between a line and the x axis.
  static double line_angle (const vgl_homg_line_2d<Type>& line);

  //: Get the line through two points (the cross-product).
  static vgl_homg_line_2d<Type> join (const vgl_homg_point_2d<Type>& point1,
                                      const vgl_homg_point_2d<Type>& point2);

  //: Get the line through two points (the cross-product).
  // In this case, we assume
  // that the points are oriented, and ensure the cross is computed with positive point
  // omegas.
  static vgl_homg_line_2d<Type> join_oriented (const vgl_homg_point_2d<Type>& point1,
                                               const vgl_homg_point_2d<Type>& point2);

  //: Get the intersection point of two lines (the cross-product).
  static vgl_homg_point_2d<Type> intersection (const vgl_homg_line_2d<Type>& line1,
                                               const vgl_homg_line_2d<Type>& line2);

  //: Get the perpendicular line to line which passes through point.
  // Params are line \f$(a,b,c)\f$ and point \f$(x,y,1)\f$.
  // Then the cross product of \f$(x,y,1)\f$ and the line's direction \f$(a,b,0)\f$,
  // called \f$(p,q,r)\f$ satisfies
  //
  //   \f$ap+bq=0\f$ (perpendicular condition) and
  //
  //   \f$px+qy+r=0\f$ (incidence condition).
  static vgl_homg_line_2d<Type> perp_line_through_point (const vgl_homg_line_2d<Type>& line,
                                                         const vgl_homg_point_2d<Type>& point);

  //: Get the perpendicular projection of point onto line.
  static vgl_homg_point_2d<Type> perp_projection (const vgl_homg_line_2d<Type>& line,
                                                  const vgl_homg_point_2d<Type>& point);

  //: Return the midpoint of the line joining two homogeneous points
  static vgl_homg_point_2d<Type> midpoint (const vgl_homg_point_2d<Type>& p1,
                                           const vgl_homg_point_2d<Type>& p2);

  //: Intersect a set of 2D lines to find the least-square point of intersection.
  static vgl_homg_point_2d<Type> lines_to_point(const vcl_vector<vgl_homg_line_2d<Type> >& lines);

  //: cross ratio of four colinear points
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
  // In principle, any single nonhomogeneous coordinate from the four points
  // can be used as parameters for cross_ratio (but of course the same for all
  // points). The most reliable answer will be obtained when the coordinate with
  // the largest spacing is used, i.e., the one with smallest slope.
  //
  // In this implementation, a least-squares result is calculated when the
  // points are not exactly collinear.
  //

  static double cross_ratio(const vgl_homg_point_2d<Type>& p1, const vgl_homg_point_2d<Type>& p2,
                            const vgl_homg_point_2d<Type>& p3, const vgl_homg_point_2d<Type>& p4);

  //: Conjugate point of three given collinear points.
  // If cross ratio cr is given (default: -1), the generalized conjugate point
  // returned is such that the cross ratio ((x1,x2;x3,answer)) = cr.
  static vgl_homg_point_2d<Type> conjugate(const vgl_homg_point_2d<Type>& a,
                                           const vgl_homg_point_2d<Type>& b,
                                           const vgl_homg_point_2d<Type>& c,
                                           double cr = -1.0);

  //: compute most orthogonal vector with vnl_symmetric_eigensystem
  static vnl_vector<Type> most_orthogonal_vector(const vcl_vector<vgl_homg_line_2d<Type> >& inpoints);

  //: compute most orthogonal vector with SVD
  static vnl_vector<Type> most_orthogonal_vector_svd(const vcl_vector<vgl_homg_line_2d<Type> >& lines);
};

#endif // vgl_homg_operations_2d_h
