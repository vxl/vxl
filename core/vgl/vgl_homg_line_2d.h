// This is core/vgl/vgl_homg_line_2d.h
#ifndef vgl_homg_line_2d_h
#define  vgl_homg_line_2d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief line in projective 2D space
// \author Don Hamilton, Peter Tu
//
// \verbatim
//  Modifications
//   Peter Vanroose -  6 July 2001 - Added normal(), direction() and concurrent()
//   Peter Vanroose -  4 July 2001 - Added assertions and cstr from non-homg line
//   Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_point_2d and vgl_line_2d
#include <vcl_cassert.h>
#include <vgl/vgl_vector_2d.h>

//: Represents a homogeneous 2D line.
template <class T>
class vgl_homg_line_2d
{
  //: the data associated with this line
  T a_;
  T b_;
  T c_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Default constructor (Line 1.y==0, the X axis)
  inline vgl_homg_line_2d() : a_(0), b_(1), c_(0) {}

  //: Construct from three Types.
  //  The three given numbers should not be all 0
  inline vgl_homg_line_2d(T va, T vb, T vc) : a_(va), b_(vb), c_(vc) {assert(va||vb||vc);}

  //: Construct from 3-vector.
  //  The three given numbers should not be all 0
  inline vgl_homg_line_2d(const T v[3]) : a_(v[0]), b_(v[1]), c_(v[2]) {assert(a_||b_||c_);}

  //: Construct from non-homogeneous line
  vgl_homg_line_2d<T> (vgl_line_2d<T> const& p);

  //: Construct from two distinct points (join)
  //  The two points must be distinct!
  vgl_homg_line_2d(vgl_homg_point_2d<T> const& p1, vgl_homg_point_2d<T> const& p2);

#if 0 // The defaults for these, as provided by the compiler, are all right:
  // Default copy constructor
  inline vgl_homg_line_2d(const vgl_homg_line_2d<T>& l) : a_(l.a()), b_(l.b()), c_(l.c()) {}

  // Default destructor
  inline ~vgl_homg_line_2d() {}

  // Default assignment operator
  inline vgl_homg_line_2d<T>& operator=(const vgl_homg_line_2d<T>& l) {
    set(l.a(),l.b(),l.c()); return *this;
  }
#endif

  //: the comparison operator
  inline bool operator==(vgl_homg_line_2d<T> const& l) const
  {
    return (this==&l) ||
           (a()*l.c()==c()*l.a() && b()*l.c()==c()*l.b() && b()*l.a()==a()*l.b());
  }

  inline bool operator!=(vgl_homg_line_2d<T> const& other)const{return !operator==(other);}

  // Data Access-------------------------------------------------------------

  //: Parameter a of line a*x + b*y + c*w = 0
  inline T a() const {return a_;}
  //: Parameter b of line a*x + b*y + c*w = 0
  inline T b() const {return b_;}
  //: Parameter c of line a*x + b*y + c*w = 0
  inline T c() const {return c_;}

  //: unit vector describing line direction, or (0,0) if line at infinity
  inline vgl_vector_2d<double> direction() const { return normalized(vgl_vector_2d<double>(b_,-a_)); }

  //: unit vector orthogonal to line, or (0,0) if line at infinity
  inline vgl_vector_2d<double> normal() const { return normalized(vgl_vector_2d<double>(a_,b_)); }

  //: divide all coefficients by sqrt(a^2 + b^2)
  void normalize();

  //: Set a b c.
  //  The three given numbers should not be all 0
  //  Note that it does not make sense to set a, b or c separately
  inline void set(T va, T vb, T vc) {assert(va||vb||vc); a_=va; b_=vb; c_=vc;}

  //: Return true iff this line is the line at infinity
  //  This version checks (max(|a|,|b|) <= tol * |c|
  inline bool ideal(T tol = (T)0) const
  {
#define vgl_Abs(x) (x<0?-x:x) // avoid #include of vcl_cmath.h AND vcl_cstdlib.h
    return vgl_Abs(a()) <= tol*vgl_Abs(c()) && vgl_Abs(b()) <= tol*vgl_Abs(c());
#undef vgl_Abs
  }

  //:get two points on the line
  // These two points are normally the intersections
  // with the Y axis and X axis, respectively.  When the line is parallel to one
  // of these, the point with y=1 or x=1, resp. are taken.  When the line goes
  // through the origin, the second point is (b, -a, 1).  Finally, when the line
  // is the line at infinity, the returned points are (1,0,0) and (0,1,0).
  // Thus, whenever possible, the returned points are not at infinity.
  void get_two_points(vgl_homg_point_2d<T> &p1, vgl_homg_point_2d<T> &p2) const;
};

#define l vgl_homg_line_2d<T>

//: Return true iff line is the line at infinity
//  This version checks (max(|a|,|b|) <= tol * |c|
// \relates vgl_homg_line_2d
template <class T>
inline bool is_ideal(l const& line, T tol = (T)0) { return line.ideal(tol); }

//: Are three lines concurrent, i.e., do they pass through a common point?
// \relates vgl_homg_line_2d
template <class T>
inline bool concurrent(l const& l1, l const& l2, l const& l3)
{
  return l1.a()*(l2.b()*l3.c()-l3.b()*l2.c())
        +l2.a()*(l3.b()*l1.c()-l1.b()*l3.c())
        +l3.a()*(l1.b()*l2.c()-l2.b()*l1.c())==0;
}

//: Print line equation to stream
// \relates vgl_homg_line_2d
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, l const& line);

//: Load in line parameters from stream
// \relates vgl_homg_line_2d
template <class T>
vcl_istream& operator>>(vcl_istream& s, l& line);

#undef l

#define VGL_HOMG_LINE_2D_INSTANTIATE(T) extern "please include vgl/vgl_homg_line_2d.txx first"

#endif //  vgl_homg_line_2d_h
