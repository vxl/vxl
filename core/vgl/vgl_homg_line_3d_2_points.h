// This is ./vxl/vgl/vgl_homg_line_3d_2_points.h
#ifndef vgl_homg_line_3d_2_points_h_
#define vgl_homg_line_3d_2_points_h_

//:
// \file
// \author Don HAMILTON Peter TU François BERTEL
//
// \verbatim
// Modifications
// Peter Vanroose -  4 July 2001 - constructors now use force_point2_infinite()
// Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim
//

#ifdef __GNUC__
#pragma interface
#endif

#include <vcl_iosfwd.h>
#include <vgl/vgl_homg_point_3d.h> // data member of this class

//:Represents a homogeneous 3D line using two points
// A class to hold a homogeneous representation of a 3D Line.  The line is
// stored as a pair of homogeneous 3d points.
template <class Type>
class vgl_homg_line_3d_2_points
{
  //+**************************************************************************
  // Initialization
  //+**************************************************************************
public:
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
  inline bool ideal(Type tol = Type(0)) { return point_finite_.ideal(tol); }

protected:
  //: force the point point_infinite_ to infinity, without changing the line
  // This is called by the constructors
  void force_point2_infinite(void) const; // mutable const

  // Internals
private:
  // Data Members------------------------------------------------------------

  //: Any finite point on the line
  mutable vgl_homg_point_3d<Type> point_finite_;
  //: the (unique) point at infinity
  mutable vgl_homg_point_3d<Type> point_infinite_;
};

//: Return true iff line is at infinity
template <class Type>
inline bool is_ideal(vgl_homg_line_3d_2_points<Type> const& line, Type tol=Type(0))
{ return line.ideal(tol); }

//+****************************************************************************
// stream operators
//+****************************************************************************

//: Write to stream (verbose)
template <class Type>
vcl_ostream &operator<<(vcl_ostream&s, vgl_homg_line_3d_2_points<Type>const&p);

//: Read parameters from stream
template <class Type>
vcl_istream &operator>>(vcl_istream &is, vgl_homg_line_3d_2_points<Type> &p);

#define VGL_HOMG_LINE_3D_2_POINTS_INSTANTIATE(T) extern "please include vgl/vgl_homg_line_3d_2_points.txx first"

#endif // vgl_homg_line_3d_2_points_h_
