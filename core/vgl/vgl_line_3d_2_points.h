// This is core/vgl/vgl_line_3d_2_points.h
#ifndef vgl_line_3d_2_points_h_
#define vgl_line_3d_2_points_h_
//:
// \file
// \brief non-homogeneous 3D line, represented by 2 points.
// \author Peter Vanroose
//
// \verbatim
//  Modifications
//   Gamze Tunali    26 Jan 2007: Deprecated intersection(). Moved into vgl_intersection.
//   Peter Vanroose  30 Mar 2007: Commented out deprecated intersection() functions.
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_point_3d.h> // data member of this class
#include <vgl/vgl_vector_3d.h>

//: A class to hold a non-homogeneous representation of a 3D line.
// The line is stored as a pair of non-homogeneous 3D points.
template <class Type>
class vgl_line_3d_2_points
{
  // Data Members------------------------------------------------------------

  //: Any point on the line
  vgl_point_3d<Type> point1_;
  //: Any other point on the line
  vgl_point_3d<Type> point2_;

 public:
  //+**************************************************************************
  // Initialization
  //+**************************************************************************

  //: Default constructor with (0,0,0) and (1,0,0), which is the line \a y=z=0
  inline vgl_line_3d_2_points(void)
  : point1_(0,0,0), point2_(1,0,0) {}

  //: Copy constructor
  inline vgl_line_3d_2_points(const vgl_line_3d_2_points<Type> &that)
  : point1_(that.point1_), point2_(that.point2_) {}

  //: Construct from two points
  inline vgl_line_3d_2_points(vgl_point_3d<Type> const& p1,
                              vgl_point_3d<Type> const& p2)
  : point1_(p1), point2_(p2) {assert(p1!=p2);}

  //: comparison
  bool operator==(vgl_line_3d_2_points<Type> const& l) const;
  bool operator!=(vgl_line_3d_2_points<Type> const& l) const{return !operator==(l);}

  // Data access

  //: Return the first point representing this line
  inline vgl_point_3d<Type> point1() const {return point1_;}
  //: Return the second point representing this line
  inline vgl_point_3d<Type> point2() const{ return point2_;}

  //: Assignment
  inline void set(vgl_point_3d<Type> const& p1, vgl_point_3d<Type> const& p2)
  { assert(p1!=p2); point1_ = p1; point2_ = p2; }

  // Utility methods

  //: Return true iff line is at infinity (which is always false)
  inline bool ideal(Type  /*tol*/ = (Type)0) const { return false; }

  //: Return the direction vector of this line (not normalised - but perhaps it should be, like other line classes?)
  inline vgl_vector_3d<Type> direction() const { return point2()-point1(); }

  //: Return a point on the line defined by a scalar parameter \a t such that \a t=0.0 at point1 and \a t=1.0 at point2.
  //\note Assumes that direction() is not normalized.
  inline vgl_point_3d<Type> point_t(const double t) const { return point1() + t*direction(); }
};

#define l vgl_line_3d_2_points<Type>

//: Return true iff line is at infinity (which is always false)
// \relatesalso vgl_line_3d_2_points
template <class Type>
inline bool is_ideal(l const&, Type=(Type)0) { return false; }

//: Does a line pass through a point, i.e., are the point and the line collinear?
// \relatesalso vgl_line_3d_2_points
// \relatesalso vgl_point_3d
template <class Type>
inline bool collinear(l const& l1, vgl_point_3d<Type> const& p)
{
  return collinear(l1.point1(),l1.point2(),p);
}

//: Are two lines coplanar, i.e., do they either intersect or are parallel?
// \relatesalso vgl_line_3d_2_points
template <class Type>
inline bool coplanar(l const& l1, l const& l2)
{ return coplanar(l1.point1(),l1.point2(),l2.point1(),l2.point2()); }

//: Are two lines concurrent, i.e., do they intersect in a finite point?
// \relatesalso vgl_line_3d_2_points
template <class Type>
inline bool concurrent(l const& l1, l const& l2)
{
  return coplanar(l1,l2) && !parallel(l1.direction(),l2.direction());
}

//: Are two points coplanar with a line?
// \relatesalso vgl_line_3d_2_points
// \relatesalso vgl_point_3d
template <class Type>
inline bool coplanar(l const& l1, vgl_point_3d<Type> const& p1, vgl_point_3d<Type> const& p2)
{ return coplanar(l1.point1(),l1.point2(),p1,p2); }

//: Are three lines coplanar, i.e., are they in a common plane?
// \relatesalso vgl_line_3d_2_points
template <class Type>
inline bool coplanar(l const& l1, l const& l2, l const& l3)
{
  vgl_point_3d<Type> p = l2.point1();
  if (collinear(l1,p)) p = l2.point2();
  return coplanar(l1,l2) && coplanar(l1,l3) &&
         coplanar(l1,p,l3.point1()) && coplanar(l1,p,l3.point2());
}

#if 0 // deprecated
//: Return the intersection point of two concurrent lines
// \relatesalso vgl_line_3d_2_points
// \deprecated in favour of vgl_intersection.
// Can be removed after the release of VXL 1.8
template <class Type>
vgl_point_3d<Type> intersection(l const& l1, l const& l2)
{ return vgl_intersection(l1, l2); }

//: Return the intersection point of a line and a plane.
// \relatesalso vgl_line_3d_2_points
// \deprecated in favour of vgl_intersection.
// Can be removed after the release of VXL 1.8
template <class Type>
vgl_point_3d<Type> intersection(l const& line, vgl_plane_3d<Type> const& plane)
{ return vgl_intersection(line, plane); }
#endif // 0

//: Are three lines concurrent, i.e., do they pass through a common point?
// \relatesalso vgl_line_3d_2_points
template <class Type>
inline bool concurrent(l const& l1, l const& l2, l const& l3)
{
  if (!concurrent(l1,l2) || !concurrent(l1,l3) || !concurrent(l2,l3)) return false;
  return vgl_intersection(l1,l2) == vgl_intersection(l1,l3);
}

//+****************************************************************************
// stream operators
//+****************************************************************************

//: Write to stream (verbose)
// \relatesalso vgl_line_3d_2_points
template <class Type>
std::ostream &operator<<(std::ostream&s, l const& );

//: Read parameters from stream
// \relatesalso vgl_line_3d_2_points
template <class Type>
std::istream &operator>>(std::istream &is, l &);

#undef l

#define VGL_LINE_3D_2_POINTS_INSTANTIATE(T) extern "please include vgl/vgl_line_3d_2_points.hxx first"

#endif // vgl_line_3d_2_points_h_
