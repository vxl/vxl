// This is core/vgl/vgl_homg_point_1d.h
#ifndef vgl_homg_point_1d_h_
#define vgl_homg_point_1d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a point in homogeneous 1-D space, i.e., a homogeneous pair \a (x,w)
// \author Peter Vanroose
// \date   8 July 2001
//
// \verbatim
// Modifications
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

//: Represents a homogeneous 1-D point, i.e., a homogeneous pair \a (x,w)
template <class T>
class vgl_homg_point_1d
{
  T x_;
  T w_;

 public:
  //: Default constructor with (0,1)
  inline vgl_homg_point_1d() : x_(0), w_(T(1)) {}

  //: Construct from one (nonhomogeneous) or two (homogeneous) T's.
  inline vgl_homg_point_1d(T px, T pw = T(1)) : x_(px), w_(pw) {}

  //: Construct from homogeneous 2-array.
  inline vgl_homg_point_1d(const T v[2]) : x_(v[0]), w_(v[1]) {}

#if 0 // the compiler defaults are better...
  // Default copy constructor
  inline vgl_homg_point_1d(const vgl_homg_point_1d<T>& that) : x_(p.x()), w_(p.w()) {}

  // Destructor
  inline ~vgl_homg_point_1d() {}

  // Default assignment operator
  inline vgl_homg_point_1d<T>& operator=(const vgl_homg_point_1d<T>& p) {
    set(p.x(),p.w()); return *this; }
#endif

  //: comparison
  inline bool operator==(vgl_homg_point_1d<T> const& p) const {
    return this==&p || x()*p.w() == w()*p.x(); }
  inline bool operator!=(vgl_homg_point_1d<T> const& p)const { return !operator==(p); }

  // Data Access-------------------------------------------------------------

  inline T x() const { return x_; }
  inline T w() const { return w_; }

  //: Set \a x,w
  // Note that it does not make sense to set \a x or \a w individually.
  inline void set(T px, T pw) { x_ = px, w_ = pw; }
  inline void set(T const p[2]) { x_ = p[0]; w_ = p[1]; }

  //: Return true iff the point is at infinity (an ideal point).
  // The method checks whether |w| <= tol * |x|
  inline bool ideal(T tol = T(0)) const {
#define vgl_Abs(x) (x<0?-x:x) // avoid #include of vcl_cmath.h AND vcl_cstdlib.h
    return vgl_Abs(w()) <= tol * vgl_Abs(x());
#undef vgl_Abs
  }
};

//  +-+-+ point_1d simple I/O +-+-+

//: Write "<vgl_homg_point_1d (x,w) > " to stream
// \relates vgl_homg_point_1d
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, vgl_homg_point_1d<T> const& p);

//: Read x w from stream
// \relates vgl_homg_point_1d
template <class T>
vcl_istream& operator>>(vcl_istream& s, vgl_homg_point_1d<T>& p);

//  +-+-+ homg_point_1d arithmetic +-+-+

//: Return true iff the point is at infinity (an ideal point).
// The method checks whether |w| <= tol * |x|
// \relates vgl_homg_point_1d
template <class T> inline
bool is_ideal(vgl_homg_point_1d<T> const& p, T tol = T(0)) { return p.ideal(tol); }

//: The difference of two points is the distance between the two.
// This function is only valid if the points are not at infinity.
// \relates vgl_homg_point_1d
template <class T> inline
T operator-(vgl_homg_point_1d<T> const& p1,
            vgl_homg_point_1d<T> const& p2) {
  assert(p1.w() && p2.w());
  return p1.x()/p1.w()-p2.x()/p2.w();
}

//: Adding a number to a 1-D point translates that point.
// If the point is at infinity, nothing happens.
// Note that number + point is not defined!  It's always point + number.
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T> operator+(vgl_homg_point_1d<T> const& p, T v)
{ return vgl_homg_point_1d<T>(p.x()+v*p.w(), p.w()); }

//: Adding a number to a 1-D point translates that point.
// If the point is at infinity, nothing happens.
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T>& operator+=(vgl_homg_point_1d<T>& p, T v)
{ p.set(p.x()+v*p.w(), p.w()); return p; }

//: Subtracting a number from a point is the same as adding the inverse number
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T> operator-(vgl_homg_point_1d<T> const& p, T v)
{ return p + (-v); }

//: Subtracting a number from a point is the same as adding the inverse number
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T>& operator-=(vgl_homg_point_1d<T>& p, T v)
{ return p += (-v); }

//  +-+-+ homg_point_1d geometry +-+-+

//: cross ratio of four points
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
// \relates vgl_homg_point_1d

template <class T> inline
double cross_ratio(vgl_homg_point_1d<T>const& p1, vgl_homg_point_1d<T>const& p2,
                   vgl_homg_point_1d<T>const& p3, vgl_homg_point_1d<T>const& p4)
{ return (p1.x()*p3.w()-p3.x()*p1.w())*(p2.x()*p4.w()-p4.x()*p2.w())
       /((p1.x()*p4.w()-p4.x()*p1.w())*(p2.x()*p3.w()-p3.x()*p2.w())); }

//: Return the relative distance to p1 wrt p1-p2 of p3.
//  p2 should not equal p1.
//  This is the coordinate of p3 in the affine 1D reference frame (p1,p2).
//  If p3=p1, the ratio is 0; if p1=p3, the ratio is 1.
//  The mid point of p1 and p2 has ratio 0.5.
//  Note that the return type is double, not T, since the ratio of e.g.
//  two int's need not be an int.
// \relates vgl_homg_point_1d
template <class T> inline
double ratio(vgl_homg_point_1d<T> const& p1,
             vgl_homg_point_1d<T> const& p2,
             vgl_homg_point_1d<T> const& p3)
{ return (p3-p1)/(p2-p1); }

//: Are three points collinear?  This is always true.
// \relates vgl_homg_point_1d
template <class T> inline
bool collinear(vgl_homg_point_1d<T> const&,
               vgl_homg_point_1d<T> const&,
               vgl_homg_point_1d<T> const&)
{ return true; }

//: Return the point at a given ratio wrt two other points.
//  By default, the mid point (ratio=0.5) is returned.
//  Note that the third argument is T, not double, so the midpoint of e.g.
//  two vgl_homg_point_1d<int> is not a valid concept.  But the reflection point
//  of p2 wrt p1 is: in that case f=-1.
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T> midpoint(vgl_homg_point_1d<T> const& p1,
                              vgl_homg_point_1d<T> const& p2,
                              T f = 0.5)
{ return p1 + f*(p2-p1); }

//: Return the point at the centre of gravity of two given points.
// Identical to midpoint(p1,p2).
// If one point or both points are at infinity, that point is returned.
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T> centre(vgl_homg_point_1d<T> const& p1,
                            vgl_homg_point_1d<T> const& p2)
{
  if (p1 == p2) return p1;
  return vgl_homg_point_1d<T> (p1.x()*p2.w() + p2.x()*p1.w(), p1.w()*p2.w()*2);
}

//: Return the point at the centre of gravity of a set of given points.
// There are no rounding errors when T is e.g. int, if all w() are 1.
// \relates vgl_homg_point_1d
template <class T> inline
vgl_homg_point_1d<T> centre(vcl_vector<vgl_homg_point_1d<T> > const& v)
{
  int n=v.size();
  assert(n>0); // it is *not* correct to return the point (0,1) when n==0.
  T x = 0;
  for (int i=0; i<n; ++i) x+=v[i].x()/v[i].w();
  return vgl_homg_point_1d<T>(x,T(n));
}

#define VGL_HOMG_POINT_1D_INSTANTIATE(T) extern "please include vgl/vgl_homg_point_1d.txx first"

#endif // vgl_homg_point_1d_h_
