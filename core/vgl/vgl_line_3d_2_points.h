// This is core/vgl/vgl_line_3d_2_points.h
#ifndef vgl_line_3d_2_points_h_
#define vgl_line_3d_2_points_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief non-homogeneous 3D line, represented by 2 points.
// \author Peter Vanroose

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>
#include <vgl/vgl_point_3d.h> // data member of this class
#include <vgl/vgl_vector_3d.h>

//:Represents a non-homogeneous 3D line using two points
// A class to hold a non-homogeneous representation of a 3D line.
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

  //: Return the direction vector of this line (not normalised)
  inline vgl_vector_3d<Type> direction() const { return point2()-point1(); }
};

#define l vgl_line_3d_2_points<Type>

//: Return true iff line is at infinity (which is always false)
// \relates vgl_line_3d_2_points
template <class Type>
inline bool is_ideal(l const&, Type=(Type)0) { return false; }

//: Does a line pass through a point, i.e., are the point and the line collinear?
// \relates vgl_line_3d_2_points
// \relates vgl_point_3d
template <class Type>
inline bool collinear(l const& l1, vgl_point_3d<Type> const& p)
{
  return collinear(l1.point1(),l1.point2(),p);
}

//: Are two lines coplanar, i.e., do they either intersect or are parallel?
// \relates vgl_line_3d_2_points
template <class Type>
inline bool coplanar(l const& l1, l const& l2)
{ return coplanar(l1.point1(),l1.point2(),l2.point1(),l2.point2()); }

//: Are two lines concurrent, i.e., do they intersect in a finite point?
// \relates vgl_line_3d_2_points
template <class Type>
inline bool concurrent(l const& l1, l const& l2)
{
  vgl_vector_3d<Type> v1=l1.direction(),v2=l2.direction();
  return coplanar(l1,l2) && v1*v2.length() != v2*v1.length();
}

//: Are two points coplanar with a line?
// \relates vgl_line_3d_2_points
// \relates vgl_point_3d
template <class Type>
inline bool coplanar(l const& l1, vgl_point_3d<Type> const& p1, vgl_point_3d<Type> const& p2)
{ return coplanar(l1.point1(),l1.point2(),p1,p2); }

//: Are three lines coplanar, i.e., are they in a common plane?
// \relates vgl_line_3d_2_points
template <class Type>
inline bool coplanar(l const& l1, l const& l2, l const& l3)
{
  vgl_point_3d<Type> p = l2.point1();
  if (collinear(l1,p)) p = l2.point2();
  return coplanar(l1,l2) && coplanar(l1,l3) &&
         coplanar(l1,p,l3.point1()) && coplanar(l1,p,l3.point2());
}

//: Return the intersection point of two concurrent lines
// \relates vgl_line_3d_2_points
template <class Type>
vgl_point_3d<Type> intersection(l const& l1, l const& l2);

//: Are three lines concurrent, i.e., do they pass through a common point?
// \relates vgl_line_3d_2_points
template <class Type>
inline bool concurrent(l const& l1, l const& l2, l const& l3)
{
  if (!concurrent(l1,l2) || !concurrent(l1,l3) || !concurrent(l2,l3)) return false;
  return intersection(l1,l2) == intersection(l1,l3);
}

//+****************************************************************************
// stream operators
//+****************************************************************************

//: Write to stream (verbose)
// \relates vgl_line_3d_2_points
template <class Type>
vcl_ostream &operator<<(vcl_ostream&s, l const& );

//: Read parameters from stream
// \relates vgl_line_3d_2_points
template <class Type>
vcl_istream &operator>>(vcl_istream &is, l &);

#undef l

#define VGL_LINE_3D_2_POINTS_INSTANTIATE(T) extern "please include vgl/vgl_line_3d_2_points.txx first"

#endif // vgl_line_3d_2_points_h_
