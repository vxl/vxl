// This is core/vgl/vgl_point_2d.h
#ifndef vgl_point_2d_h
#define vgl_point_2d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a point in 2D nonhomogeneous space
// \author Don Hamilton, Peter Tu
//
// \verbatim
//  Modifications
//   29 June 2001 Peter Vanroose moved arithmetic operators to new vgl_vector_2d
//    2 July 2001 Peter Vanroose implemented constructor from homg point
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // declare vgl_homg_point_2d and vgl_line_2d
#include <vgl/vgl_vector_2d.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

//: Represents a cartesian 2D point
template <class Type>
class vgl_point_2d
{
  // the data associated with this point
  Type x_;
  Type y_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Default constructor
  inline vgl_point_2d () {}

  //: Construct from two Types.
  inline vgl_point_2d (Type px, Type py) : x_(px), y_(py) {}

  //: Construct from 2-array.
  inline vgl_point_2d (Type const v[2]) : x_(v[0]), y_(v[1]) {}

  //: Construct from homogeneous point
  vgl_point_2d (vgl_homg_point_2d<Type> const& p);

  //: Construct from 2 lines (intersection).
  vgl_point_2d (vgl_line_2d<Type> const& l1,
                vgl_line_2d<Type> const& l2);

#if 0 // The compiler defaults for these are doing what they should do:
  //: Copy constructor
  inline vgl_point_2d(vgl_point_2d<Type> const& p) : x_(p.x()), y_(p.y()) {}
  //: Destructor
  inline ~vgl_point_2d () {}
  //: Assignment
  inline vgl_point_2d<Type>& operator=(const vgl_point_2d<Type>& p)
  { x_ = p.x(); y_ = p.y(); return *this; }
#endif

  //: Test for equality
  inline bool operator==(vgl_point_2d<Type>const& p) const
  { return this==&p || (x_==p.x() && y_==p.y()); }
  inline bool operator!=(vgl_point_2d<Type>const& p)const
  { return !operator==(p); }

  // Data Access-------------------------------------------------------------

  inline Type &x() {return x_;}
  inline Type &y() {return y_;}

  inline Type x() const {return x_;}
  inline Type y() const {return y_;}

  //: Set \a x and \a y
  //  Note that \a x and \a y can also be set individually.
  inline void set (Type px, Type py){ x_ = px; y_ = py; }
  //: Set \a x and \a y
  //  Note that \a x and \a y can also be set individually.
  inline void set (Type const p[2]) { x_ = p[0]; y_ = p[1]; }

  //: Return true iff the point is at infinity (an ideal point).
  //  Always returns false.
  inline bool ideal(Type = (Type)0) const { return false; }
};

//  +-+-+ point_2d simple I/O +-+-+

//: Write "<vgl_point_2d x,y>" to stream
// \relates vgl_point_2d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_point_2d<Type> const& p);

//: Read x y from stream
// \relates vgl_point_2d
template <class Type>
vcl_istream&  operator>>(vcl_istream& s, vgl_point_2d<Type>& p);

//  +-+-+ point_2d arithmetic +-+-+

//: Return true iff the point is at infinity (an ideal point).
//  Always returns false.
template <class Type> inline
bool is_ideal(vgl_point_2d<Type> const&, Type = 0) { return false; }

//: The difference of two points is the vector from second to first point
// \relates vgl_point_2d
template <class Type> inline
vgl_vector_2d<Type> operator-(vgl_point_2d<Type> const& p1,
                              vgl_point_2d<Type> const& p2)
{ return vgl_vector_2d<Type>(p1.x()-p2.x(), p1.y()-p2.y()); }

//: Adding a vector to a point gives a new point at the end of that vector
// Note that vector + point is not defined!  It's always point + vector.
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> operator+(vgl_point_2d<Type> const& p,
                             vgl_vector_2d<Type> const& v)
{ return vgl_point_2d<Type>(p.x()+v.x(), p.y()+v.y()); }

//: Adding a vector to a point gives the point at the end of that vector
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type>& operator+=(vgl_point_2d<Type>& p,
                               vgl_vector_2d<Type> const& v)
{ p.set(p.x()+v.x(), p.y()+v.y()); return p; }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> operator-(vgl_point_2d<Type> const& p,
                             vgl_vector_2d<Type> const& v)
{ return p + (-v); }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type>& operator-=(vgl_point_2d<Type>& p,
                               vgl_vector_2d<Type> const& v)
{ return p += (-v); }

//  +-+-+ point_2d geometry +-+-+

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
// If three of the given points coincide, the cross ratio is not defined.
//
// In this implementation, a least-squares result is calculated when the
// points are not exactly collinear.
//
// \relates vgl_point_2d
template <class T>
double cross_ratio(vgl_point_2d<T>const& p1, vgl_point_2d<T>const& p2,
                   vgl_point_2d<T>const& p3, vgl_point_2d<T>const& p4);

//: Are three points collinear, i.e., do they lie on a common line?
// \relates vgl_point_2d
template <class Type> inline
bool collinear(vgl_point_2d<Type> const& p1,
               vgl_point_2d<Type> const& p2,
               vgl_point_2d<Type> const& p3)
{ return parallel(p1-p2, p1-p3); }

//: Return the relative distance to p1 wrt p1-p2 of p3.
//  The three points should be collinear and p2 should not equal p1.
//  This is the coordinate of p3 in the affine 1D reference frame (p1,p2).
//  If p3=p1, the ratio is 0; if p1=p3, the ratio is 1.
//  The mid point of p1 and p2 has ratio 0.5.
//  Note that the return type is double, not Type, since the ratio of e.g.
//  two vgl_vector_2d<int> need not be an int.
// \relates vgl_point_2d
template <class Type> inline
double ratio(vgl_point_2d<Type> const& p1,
             vgl_point_2d<Type> const& p2,
             vgl_point_2d<Type> const& p3)
{ return (p3-p1)/(p2-p1); }

//: Return the point at a given ratio wrt two other points.
//  By default, the mid point (ratio=0.5) is returned.
//  Note that the third argument is Type, not double, so the midpoint of e.g.
//  two vgl_point_2d<int> is not a valid concept.  But the reflection point
//  of p2 wrt p1 is: in that case f=-1.
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> midpoint(vgl_point_2d<Type> const& p1,
                            vgl_point_2d<Type> const& p2,
                            Type f = (Type)0.5)
{
  return vgl_point_2d<Type>((Type)((1-f)*p1.x() + f*p2.x()),
                            (Type)((1-f)*p1.y() + f*p2.y()));
}


//: Return the point at the centre of gravity of two given points.
// Identical to midpoint(p1,p2).
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> centre(vgl_point_2d<Type> const& p1,
                          vgl_point_2d<Type> const& p2)
{
  return vgl_point_2d<Type>((p1.x() + p2.x())/2 ,
                            (p1.y() + p2.y())/2 );
}

//: Return the point at the centre of gravity of three given points.
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> centre(vgl_point_2d<Type> const& p1,
                          vgl_point_2d<Type> const& p2,
                          vgl_point_2d<Type> const& p3)
{
  return vgl_point_2d<Type>((p1.x() + p2.x() + p3.x())/3 ,
                            (p1.y() + p2.y() + p3.y())/3 );
}

//: Return the point at the centre of gravity of four given points.
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> centre(vgl_point_2d<Type> const& p1,
                          vgl_point_2d<Type> const& p2,
                          vgl_point_2d<Type> const& p3,
                          vgl_point_2d<Type> const& p4)
{
  return vgl_point_2d<Type>((p1.x() + p2.x() + p3.x() + p4.x())/4 ,
                            (p1.y() + p2.y() + p3.y() + p4.y())/4 );
}

//: Return the point at the centre of gravity of a set of given points.
// Beware of possible rounding errors when Type is e.g. int.
// \relates vgl_point_2d
template <class Type> inline
vgl_point_2d<Type> centre(vcl_vector<vgl_point_2d<Type> > const& v)
{
  int n=v.size();
  assert(n>0); // it is *not* correct to return the point (0,0) when n==0.
  Type x = 0, y = 0;
  for (int i=0; i<n; ++i) x+=v[i].x(), y+=v[i].y();
  return vgl_point_2d<Type>(x/n,y/n);
}

#define VGL_POINT_2D_INSTANTIATE(T) extern "please include vgl/vgl_point_2d.txx first"

#endif // vgl_point_2d_h
