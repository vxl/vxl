// This is core/vgl/vgl_line_2d.h
#ifndef vgl_line_2d_h_
#define vgl_line_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Don Hamilton, Peter Tu, Peter Vanroose, François BERTEL, Franck Bettinger
// \date   2000/02/16 Don HAMILTON, Peter TU - Creation
//
// \verbatim
//  Modifications
//   2000/02/29 Peter Vanroose    Several minor fixes
//   2000/05/05 François BERTEL   Several minor bugs fixed
//   2000/05/09 Peter Vanroose    dist_origin() re-implemented
//   2000/12/01 Peter Vanroose    moved dist_origin() to vgl_distance.h
//   2001/03/19 Franck Bettinger  added Manchester binary IO code
//   2001/06/27 Peter Vanroose    Added operator==
//   2001/07/05 Peter Vanroose    direction, normal in terms of vgl_vector_2d
//   2001/07/06 Peter Vanroose    Added concurrent(), added assertions
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_point_2d and vgl_homg_line_2d
#include <vgl/vgl_vector_2d.h>

//: Represents a Euclidean 2D line
// An interface for the line coefficients, [a,b,c], is provided in terms of the
// standard implicit line equation: a*x + b*y + c = 0
template <class Type>
class vgl_line_2d
{
  // the data associated with this point
  Type a_;
  Type b_;
  Type c_;

 public:
  //: Default constructor (Line 1.y==0, the X axis)
  inline vgl_line_2d() : a_(0), b_(1), c_(0) {}

  //: Construct a vgl_line_2d from its equation, three Types.
  //  The values of a and b should not be both zero.
  inline vgl_line_2d(Type a, Type b, Type c) :a_(a),b_(b),c_(c){assert(a||b);}

  //: Construct from its equation, a 3-vector.
  //  The values v[0] and v[1] should not be both zero.
  inline vgl_line_2d(const Type v[3]):a_(v[0]),b_(v[1]),c_(v[2]){assert(a_||b_);}

  //: Construct from homogeneous description of line
  //  The line l should not be the line at infinity.
  vgl_line_2d (vgl_homg_line_2d<Type> const& l);

  //: Construct from two distinct points (join)
  //  The two points must be distinct!
  vgl_line_2d (vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2);

#if 0 // use compiler defaults for these
  // Default destructor
  inline ~vgl_line_2d () {}

  // Default assignment operator
  inline vgl_line_2d<Type>& operator=(const vgl_line_2d<Type>& l)
  { set(l.a(),l.b(),l.c()); return *this; }
#endif

  //: the comparison operator
  inline bool operator==(vgl_line_2d<Type> const& l) const
  {
    return (this==&l) ||
           (a()*l.c()==c()*l.a() && b()*l.c()==c()*l.b() && b()*l.a()==a()*l.b());
  }

  inline bool operator!=(vgl_line_2d<Type>const& other)const{return !operator==(other);}

  //: angle with the horizontal line y=0, measured in radians.
  //  Returns values between -pi and pi, i.e., the lines x-y=0 and y-x=0
  //  return different values (pi/4 and -3pi/4 respectively) although these
  //  lines are identical.
  double slope_radians() const;

  //: angle with the horizontal line y=0, measured in 360-degrees.
  //  Returns values between -180 and 180, i.e., the lines x-y=0 and y-x=0
  //  return different values (45 and -135 respectively) although these
  //  lines are identical.
  double slope_degrees() const;

  // Data Access-------------------------------------------------------------

  //: Parameter a of line a*x + b*y + c = 0
  inline Type a() const {return a_;}
  //: Parameter b of line a*x + b*y + c = 0
  inline Type b() const {return b_;}
  //: Parameter c of line a*x + b*y + c = 0
  inline Type c() const {return c_;}

  //: unit vector describing line direction
  inline vgl_vector_2d<double> direction() const
  { return normalized(vgl_vector_2d<double>(b_,-a_)); }

  //: unit vector orthogonal to line
  inline vgl_vector_2d<double> normal() const
  { return normalized(vgl_vector_2d<double>(a_,b_)); }

  //: normalize the line coefficients s.t. a^2 + b^2 = 1
  bool normalize();

  //: Set a b c.
  //  The values of a and b should not be both zero.
  //  Note that it does not make sense to set a, b or c separately
  inline void set (Type a, Type b, Type c){ assert(a||b); a_=a; b_=b; c_=c; }

  //: Return true iff this line is the line at infinity
  //  This always returns "false"
  inline bool ideal(Type = (Type)0) const { return false; }

  //: Get two points on the line; normally the intersection with X and Y axes.
  // When the line is parallel to one of these,
  // the point with \a y=1 or \a x=1, resp. are taken.  When the line goes
  // through the origin, the second point is (b, -a).
  void get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2) const;
};

#define l vgl_line_2d<Type>

//: Return true iff line is the line at infinity
// \relates vgl_line_2d
template <class Type> inline
bool is_ideal(l const&, Type = (Type)0) { return false; }

//: Are three lines concurrent, i.e., do they pass through a common point?
// \relates vgl_line_2d
template <class Type> inline
bool concurrent(l const& l1, l const& l2, l const& l3)
{
  return l1.a()*(l2.b()*l3.c()-l3.b()*l2.c())
        +l2.a()*(l3.b()*l1.c()-l1.b()*l3.c())
        +l3.a()*(l1.b()*l2.c()-l2.b()*l1.c())==0;
}

//: Write line description to stream: "<vgl_line_2d ax+by+c>"
// \relates vgl_line_2d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, l const& line);

//: Read in three line parameters from stream
// \relates vgl_line_2d
template <class Type>
vcl_istream&  operator>>(vcl_istream& s, l& line);

#undef l

#define VGL_LINE_2D_INSTANTIATE(T) extern "please include vgl/vgl_line_2d.txx first"

#endif // vgl_line_2d_h_
