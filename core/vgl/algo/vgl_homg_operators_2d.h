// This is core/vgl/algo/vgl_homg_operators_2d.h
#ifndef vgl_homg_operations_2d_h
#define vgl_homg_operations_2d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief 2D homogeneous operations
// \author Don Hamilton, Peter Tu
// \date   Feb 16 2000
//
// \verbatim
// Modifications
//   31-Oct-00 Peter Vanroose - signatures fixed, and vcl_list iterator used
//   16-Mar-01 Tim Cootes - added documentation
//   29-Aug-01 Peter Vanroose - added vgl_conic functions (ported from TargetJr)
//    5-Oct-01 Peter Vanroose - added compute_bounding_box functions
//   15-May-03 Peter Vanroose - added implementation for closest_point()
//   22-Jun-03 Peter Vanroose - vcl_list replaced by vcl_vector in lines_to_point
// \endverbatim

#include <vcl_list.h>
#include <vcl_vector.h>
#ifdef VCL_VC60
# include <vcl_limits.h>
#endif
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>

//: 2D homogeneous operations
template <class T>
class vgl_homg_operators_2d
{
 public:
  //: get a vnl_vector representation of a homogeneous object
  static vnl_vector<T> get_vector(vgl_homg_point_2d<T> const& p);

  //: get a vnl_vector representation of a homogeneous object
  static vnl_vector<T> get_vector(vgl_homg_line_2d<T> const& l);

  //: get a vnl_vector representation of a homogeneous object
  static vnl_vector<T> get_vector(vgl_conic<T> const& c);

  //: Normalize vgl_homg_point_2d<T> to unit magnitude
  static void unitize(vgl_homg_point_2d<T>& a);

  static double angle_between_oriented_lines(const vgl_homg_line_2d<T>& line1,
                                             const vgl_homg_line_2d<T>& line2);
  //: Get the 0 to pi/2 angle between two lines
  static double abs_angle(const vgl_homg_line_2d<T>& line1,
                          const vgl_homg_line_2d<T>& line2);

  //: Get the square of the 2D distance between the two points.
  static double distance_squared(const vgl_homg_point_2d<T>& point1, const vgl_homg_point_2d<T>& point2);

  //: Get the square of the perpendicular distance to a line.
  // This is just the homogeneous form of the familiar
  // \f$ \frac{a x + b y + c}{\sqrt{a^2+b^2}} \f$ :
  // \[ d = \frac{(l^\top p)}{p_z\sqrt{l_x^2 + l_y^2}} \]
  // If either the point or the line are at infinity an error message is
  // printed and Homg::infinity is returned.
  static double perp_dist_squared(const vgl_homg_point_2d<T>& point,
                                  const vgl_homg_line_2d<T>& line);
  static double perp_dist_squared(const vgl_homg_line_2d<T>& line,
                                  const vgl_homg_point_2d<T>& point)
  { return perp_dist_squared(point, line); }

  //: True if the points are closer than Euclidean distance d.
  static bool is_within_distance(const vgl_homg_point_2d<T>& p1,
                                 const vgl_homg_point_2d<T>& p2, double d)
  {
    return distance_squared(p1, p2) < d*d;
  }

  //: Get the anticlockwise angle between a line and the \a x axis.
  static double line_angle(const vgl_homg_line_2d<T>& line);

  //: Get the line through two points (the cross-product).
  static vgl_homg_line_2d<T> join(const vgl_homg_point_2d<T>& point1,
                                  const vgl_homg_point_2d<T>& point2);

  //: Get the line through two points (the cross-product).
  // In this case, we assume
  // that the points are oriented, and ensure the cross is computed with positive point
  // omegas.
  static vgl_homg_line_2d<T> join_oriented(const vgl_homg_point_2d<T>& point1,
                                           const vgl_homg_point_2d<T>& point2);

  //: Get the intersection point of two lines (the cross-product).
  static vgl_homg_point_2d<T> intersection(const vgl_homg_line_2d<T>& line1,
                                           const vgl_homg_line_2d<T>& line2);

  //: Get the perpendicular line to line which passes through point.
  // Params are line \f$(a,b,c)\f$ and point \f$(x,y,1)\f$.
  // Then the cross product of \f$(x,y,1)\f$ and the line's direction \f$(a,b,0)\f$,
  // called \f$(p,q,r)\f$ satisfies
  //
  //   \f$ap+bq=0\f$ (perpendicular condition) and
  //
  //   \f$px+qy+r=0\f$ (incidence condition).
  static vgl_homg_line_2d<T> perp_line_through_point(const vgl_homg_line_2d<T>& line,
                                                     const vgl_homg_point_2d<T>& point);

  //: Get the perpendicular projection of point onto line.
  static vgl_homg_point_2d<T> perp_projection(const vgl_homg_line_2d<T>& line,
                                              const vgl_homg_point_2d<T>& point);

  //: Return the midpoint of the line joining two homogeneous points
  static vgl_homg_point_2d<T> midpoint(const vgl_homg_point_2d<T>& p1,
                                       const vgl_homg_point_2d<T>& p2);

  //: Intersect a set of 2D lines to find the least-square point of intersection.
  static vgl_homg_point_2d<T> lines_to_point(const vcl_vector<vgl_homg_line_2d<T> >& lines);

  //: cross ratio of four collinear points
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

  static double cross_ratio(const vgl_homg_point_2d<T>& p1, const vgl_homg_point_2d<T>& p2,
                            const vgl_homg_point_2d<T>& p3, const vgl_homg_point_2d<T>& p4);

  //: Conjugate point of three given collinear points.
  // If cross ratio cr is given (default: -1), the generalized conjugate point
  // returned is such that the cross ratio ((x1,x2;x3,answer)) = cr.
  static vgl_homg_point_2d<T> conjugate(const vgl_homg_point_2d<T>& a,
                                        const vgl_homg_point_2d<T>& b,
                                        const vgl_homg_point_2d<T>& c,
                                        double cr = -1.0);

  //: compute most orthogonal vector with vnl_symmetric_eigensystem
  static vnl_vector<T> most_orthogonal_vector(const vcl_vector<vgl_homg_line_2d<T> >& lines);

  //: compute most orthogonal vector with SVD
  static vnl_vector<T> most_orthogonal_vector_svd(const vcl_vector<vgl_homg_line_2d<T> >& lines);

  // coefficient <-> conic matrix conversion -------------------------
  static vgl_conic<T> vgl_conic_from_matrix(vnl_matrix_fixed<T,3,3> const& mat);
  static vnl_matrix_fixed<T,3,3> matrix_from_conic(vgl_conic<T> const&);
  static vnl_matrix_fixed<T,3,3> matrix_from_dual_conic(vgl_conic<T> const&);

  //: Find all real intersection points of a conic and a line (between 0 and 2)
  static vcl_list<vgl_homg_point_2d<T> > intersection(vgl_conic<T> const& c,
                                                      vgl_homg_line_2d<T> const& l);

  //: Find all real intersection points of two conics (between 0 and 4)
  static vcl_list<vgl_homg_point_2d<T> > intersection(vgl_conic<T> const& c1,
                                                      vgl_conic<T> const& c2);

  //: Return the (at most) two tangent lines that pass through p and are tangent to the conic.
  static vcl_list<vgl_homg_line_2d<T> > tangent_from(vgl_conic<T> const& c,
                                                     vgl_homg_point_2d<T> const& p);

  //: Return the list of common tangent lines of two conics.
  static vcl_list<vgl_homg_line_2d<T> > common_tangents(vgl_conic<T> const& c1,
                                                        vgl_conic<T> const& c2);

  //: Return the point on the line closest to the given point
  static vgl_homg_point_2d<T> closest_point(vgl_homg_line_2d<T> const& l,
                                            vgl_homg_point_2d<T> const& p);

  //: Return the point on the conic closest to the given point
  static vgl_homg_point_2d<T> closest_point(vgl_conic<T> const& c,
                                            vgl_homg_point_2d<T> const& p);

  //: Return the point on the conic closest to the given point
  static vgl_homg_point_2d<T> closest_point(vgl_conic<T> const& c,
                                            vgl_point_2d<T> const& p);

  //: Return the shortest squared distance between the conic and the point
  inline static double distance_squared(vgl_conic<T> const& c,
                                        vgl_homg_point_2d<T> const& p) {
    return distance_squared(closest_point(c,p), p);
  }

  //: Compute the bounding box of an ellipse
  static vgl_box_2d<T> compute_bounding_box(vgl_conic<T> const& c);

 private:
  // Helper functions for conic intersection
  static vcl_list<vgl_homg_point_2d<T> > do_intersect(vgl_conic<T> const& q, vgl_homg_line_2d<T> const& l);
  static vcl_list<vgl_homg_point_2d<T> > do_intersect(vgl_conic<T> const& c1, vgl_conic<T> const& c2);
};

//: Transform a point through a 3x3 projective transformation matrix
template <class T>
vgl_homg_point_2d<T> operator*(vnl_matrix_fixed<T,3,3> const& m,
                               vgl_homg_point_2d<T> const& p);

//: Transform a line through a 3x3 projective transformation matrix
template <class T>
vgl_homg_line_2d<T> operator*(vnl_matrix_fixed<T,3,3> const& m,
                              vgl_homg_line_2d<T> const& p);

#endif // vgl_homg_operations_2d_h
