// This is core/vgl/vgl_homg_line_3d_2_points.h
#ifndef vgl_homg_line_3d_2_points_h_
#define vgl_homg_line_3d_2_points_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Don HAMILTON Peter TU François BERTEL
//
// \verbatim
//  Modifications
//   Peter Vanroose -  4 July 2001 - constructors now use force_point2_infinite()
//   Peter Vanroose - 27 June 2001 - Added operator==
//   Peter Vanroose - 15 July 2002 - Added concurrent(), coplanar() and intersection()
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_homg_point_3d.h> // data member of this class

//:Represents a homogeneous 3D line using two points
// A class to hold a homogeneous representation of a 3D Line.  The line is
// stored as a pair of homogeneous 3d points.
template <class Type>
class vgl_homg_line_3d_2_points
{
  // Data Members------------------------------------------------------------

  //: Any finite point on the line
  mutable vgl_homg_point_3d<Type> point_finite_;
  //: the (unique) point at infinity
  mutable vgl_homg_point_3d<Type> point_infinite_;

 public:
  //+**************************************************************************
  // Initialization
  //+**************************************************************************

  //: Default constructor with (0,0,0,1) and (1,0,0,0), which is the line \a y=z=0
  inline vgl_homg_line_3d_2_points(void)
  : point_finite_(0,0,0,1), point_infinite_(1,0,0,0) {}

  //: Copy constructor
  inline vgl_homg_line_3d_2_points(const vgl_homg_line_3d_2_points<Type> &that)
  : point_finite_(that.point_finite_), point_infinite_(that.point_infinite_) {}

  //: Construct from two points
  inline vgl_homg_line_3d_2_points(vgl_homg_point_3d<Type> const& point_1,
                                   vgl_homg_point_3d<Type> const& point_2)
  : point_finite_(point_1), point_infinite_(point_2) {force_point2_infinite();}

#if 0
  //: Destructor (does nothing)
  inline ~vgl_homg_line_3d_2_points() {}
#endif

  //: comparison
  bool operator==(vgl_homg_line_3d_2_points<Type> const& l) const;
  inline bool operator!=(vgl_homg_line_3d_2_points<Type> const& l) const{return !operator==(l);}

  // Data access

  //: Finite point (Could be an ideal point, if the whole line is at infinity.)
  inline vgl_homg_point_3d<Type> point_finite() const {return point_finite_;}
  //: Infinite point: the intersection of the line with the plane at infinity
  inline vgl_homg_point_3d<Type> point_infinite()const{return point_infinite_;}

  //: Assignment
  inline void set(vgl_homg_point_3d<Type> const& p1, vgl_homg_point_3d<Type> const& p2)
  { point_finite_ = p1; point_infinite_ = p2; force_point2_infinite(); }

  // Utility methods

  //: Return true iff line is at infinity
  inline bool ideal(Type tol = (Type)0) const { return point_finite_.ideal(tol); }

 protected:
  //: force the point point_infinite_ to infinity, without changing the line
  // This is called by the constructors
  void force_point2_infinite(void) const; // mutable const
};

#define l vgl_homg_line_3d_2_points<Type>

//: Return true iff line is at infinity
// \relates vgl_homg_line_3d_2_points
template <class Type>
inline bool is_ideal(l const& line, Type tol=(Type)0)
{ return line.ideal(tol); }

//: Does a line pass through a point, i.e., are the point and the line collinear?
// \relates vgl_homg_line_3d_2_points
// \relates vgl_homg_point_3d
template <class Type>
inline bool collinear(l const& l1, vgl_homg_point_3d<Type> const& p)
{ return collinear(l1.point_finite(),l1.point_infinite(),p); }

//: Are two lines coplanar, i.e., do they intersect?
// \relates vgl_homg_line_3d_2_points
template <class Type>
inline bool coplanar(l const& l1, l const& l2)
{ return coplanar(l1.point_finite(),l1.point_infinite(),l2.point_finite(),l2.point_infinite()); }

//: Are two lines concurrent, i.e., do they intersect?
// \relates vgl_homg_line_3d_2_points
template <class Type>
inline bool concurrent(l const& l1, l const& l2) { return coplanar(l1,l2); }

//: Are two points coplanar with a line?
// \relates vgl_homg_line_3d_2_points
// \relates vgl_homg_point_3d
template <class Type>
inline bool coplanar(l const& l1, vgl_homg_point_3d<Type> const& p1, vgl_homg_point_3d<Type> const& p2)
{ return coplanar(l1.point_finite(),l1.point_infinite(),p1,p2); }

//: Are three lines coplanar, i.e., are they in a common plane?
// \relates vgl_homg_line_3d_2_points
template <class Type>
inline bool coplanar(l const& l1, l const& l2, l const& l3)
{
  vgl_homg_point_3d<Type> p = l2.point_finite();
  if (collinear(l1,p)) p = l2.point_infinite();
  return coplanar(l1,l2) && coplanar(l1,l3) &&
         coplanar(l1,p,l3.point_finite()) &&
         coplanar(l1,p,l3.point_infinite());
}

//: Return the intersection point of two concurrent lines
// \relates vgl_homg_line_3d_2_points
template <class Type>
vgl_homg_point_3d<Type> intersection(l const& l1, l const& l2);

//: Are three lines concurrent, i.e., do they pass through a common point?
// \relates vgl_homg_line_3d_2_points
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
// \relates vgl_homg_line_3d_2_points
template <class Type>
vcl_ostream &operator<<(vcl_ostream&s, l const& p);

//: Read parameters from stream
// \relates vgl_homg_line_3d_2_points
template <class Type>
vcl_istream &operator>>(vcl_istream &is, l &p);

#undef l

#define VGL_HOMG_LINE_3D_2_POINTS_INSTANTIATE(T) extern "please include vgl/vgl_homg_line_3d_2_points.txx first"

#endif // vgl_homg_line_3d_2_points_h_
