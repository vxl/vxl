// This is core/vgl/vgl_homg_point_2d.h
#ifndef vgl_homg_point_2d_h_
#define vgl_homg_point_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief point in projective 2D space
// \author Don HAMILTON, Peter TU
//
// \verbatim
//  Modifications
//   Peter Vanroose -  4 July 2001 - Added geometric interface like vgl_point_2d
//   Peter Vanroose -  1 July 2001 - Renamed data to x_ y_ w_, inlined constructors
//   Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_line_2d
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

//: Represents a homogeneous 2D point
template <class T>
class vgl_homg_point_2d
{
  // the data associated with this point
  T x_;
  T y_;
  T w_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Default constructor with (0,0,1)
  inline vgl_homg_point_2d() : x_(0), y_(0), w_((T)1) {}

  //: Construct from two (nonhomogeneous) or three (homogeneous) Types.
  inline vgl_homg_point_2d(T px, T py, T pw = (T)1)
    : x_(px), y_(py), w_(pw) {}

  //: Construct from homogeneous 3-array.
  inline vgl_homg_point_2d(const T v[3]) : x_(v[0]), y_(v[1]), w_(v[2]) {}

  //: Construct point at infinity from direction vector.
  inline vgl_homg_point_2d(vgl_vector_2d<T>const& v) : x_(v.x()), y_(v.y()), w_(0) {}

  //: Construct from (non-homogeneous) vgl_point_2d<T>
  inline explicit vgl_homg_point_2d(vgl_point_2d<T> const& p)
    : x_(p.x()), y_(p.y()), w_((T)1) {}

  //: Construct from 2 lines (intersection).
  vgl_homg_point_2d(vgl_homg_line_2d<T> const& l1,
                    vgl_homg_line_2d<T> const& l2);

#if 0
  // Default copy constructor
  inline vgl_homg_point_2d(const vgl_homg_point_2d<T>& p)
    : x_(p.x()), y_(p.y()), w_(p.w()) {}

  // Destructor
  inline ~vgl_homg_point_2d() {}

  // Default assignment operator
  inline vgl_homg_point_2d<T>& operator=(vgl_homg_point_2d<T>const& p)
  {
    set(p.x(),p.y(),p.w());
    return *this;
  }
#endif

  //: the comparison operator
  inline bool operator==(vgl_homg_point_2d<T> const& p) const
  {
    return (this==&p) ||
           (x()*p.w()==w()*p.x() && y()*p.w()==w()*p.y() && y()*p.x()==x()*p.y());
  }

  inline bool operator!=(vgl_homg_point_2d<T> const& other)const{return !operator==(other);}

  // Data Access-------------------------------------------------------------

  inline T x() const { return x_; }
  inline T y() const { return y_; }
  inline T w() const { return w_; }

  //: Set \a x,y,w
  // Note that it does not make sense to set \a x, \a y or \a w individually.
  inline void set(T px, T py, T pw = (T)1)
  { x_ = px, y_ = py, w_ = pw; }

  inline void set(T const p[3]) { x_ = p[0]; y_ = p[1]; w_ = p[2]; }

  //: Return true iff the point is at infinity (an ideal point).
  // The method checks whether |w| <= tol * max(|x|,|y|)
  inline bool ideal(T tol = (T)0) const
  {
#define vgl_Abs(x) (x<0?-x:x) // avoid #include of vcl_cmath.h AND vcl_cstdlib.h
    return vgl_Abs(w()) <= tol * vgl_Abs(x()) ||
           vgl_Abs(w()) <= tol * vgl_Abs(y());
#undef vgl_Abs
  }
};

//  +-+-+ point_2d simple I/O +-+-+

//: Write "<vgl_homg_point_2d (x,y,w) >" to stream
// \relates vgl_homg_point_2d
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, vgl_homg_point_2d<T> const& p);

//: Read x y w from stream
// \relates vgl_homg_point_2d
template <class T>
vcl_istream& operator>>(vcl_istream& s, vgl_homg_point_2d<T>& p);

//  +-+-+ homg_point_2d arithmetic +-+-+

//: Return true iff the point is at infinity (an ideal point).
// The method checks whether |w| <= tol * max(|x|,|y|)
// \relates vgl_homg_point_2d
template <class T> inline
bool is_ideal(vgl_homg_point_2d<T> const& p, T tol=(T)0){return p.ideal(tol);}

//: The difference of two points is the vector from second to first point
// This function is only valid if the points are not at infinity.
// \relates vgl_homg_point_2d
template <class T> inline
vgl_vector_2d<T> operator-(vgl_homg_point_2d<T> const& p1,
                           vgl_homg_point_2d<T> const& p2)
{
  assert(p1.w() && p2.w());
  return vgl_vector_2d<T>(p1.x()/p1.w()-p2.x()/p2.w(),
                          p1.y()/p1.w()-p2.y()/p2.w());
}

//: Adding a vector to a point gives a new point at the end of that vector
// If the point is at infinity, nothing happens.
// Note that vector + point is not defined!  It's always point + vector.
// \relates vgl_homg_point_2d
template <class T> inline
vgl_homg_point_2d<T> operator+(vgl_homg_point_2d<T> const& p,
                               vgl_vector_2d<T> const& v)
{ return vgl_homg_point_2d<T>(p.x()+v.x()*p.w(), p.y()+v.y()*p.w(), p.w()); }

//: Adding a vector to a point gives the point at the end of that vector
// If the point is at infinity, nothing happens.
// \relates vgl_homg_point_2d
template <class T> inline
vgl_homg_point_2d<T>& operator+=(vgl_homg_point_2d<T>& p,
                                 vgl_vector_2d<T> const& v)
{ p.set(p.x()+v.x()*p.w(), p.y()+v.y()*p.w(), p.w()); return p; }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_homg_point_2d
template <class T> inline
vgl_homg_point_2d<T> operator-(vgl_homg_point_2d<T> const& p,
                               vgl_vector_2d<T> const& v)
{ return p + (-v); }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_homg_point_2d
template <class T> inline
vgl_homg_point_2d<T>& operator-=(vgl_homg_point_2d<T>& p,
                                 vgl_vector_2d<T> const& v)
{ return p += (-v); }

//  +-+-+ homg_point_2d geometry +-+-+

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
// \relates vgl_homg_point_2d
//
template <class T>
double cross_ratio(vgl_homg_point_2d<T>const& p1, vgl_homg_point_2d<T>const& p2,
                   vgl_homg_point_2d<T>const& p3, vgl_homg_point_2d<T>const& p4);

//: Are three points collinear, i.e., do they lie on a common line?
// \relates vgl_homg_point_2d
template <class T> inline
bool collinear(vgl_homg_point_2d<T> const& p1,
               vgl_homg_point_2d<T> const& p2,
               vgl_homg_point_2d<T> const& p3)
{
  return (p1.x()*p2.y()-p1.y()*p2.x())*p3.w()
        +(p3.x()*p1.y()-p3.y()*p1.x())*p2.w()
        +(p2.x()*p3.y()-p2.y()*p3.x())*p1.w()==0;
}

//: Return the relative distance to p1 wrt p1-p2 of p3.
//  The three points should be collinear and p2 should not equal p1.
//  This is the coordinate of p3 in the affine 1D reference frame (p1,p2).
//  If p3=p1, the ratio is 0; if p1=p3, the ratio is 1.
//  The mid point of p1 and p2 has ratio 0.5.
//  Note that the return type is double, not T, since the ratio of e.g.
//  two vgl_vector_2d<int> need not be an int.
// \relates vgl_homg_point_2d
template <class T> inline
double ratio(vgl_homg_point_2d<T> const& p1,
             vgl_homg_point_2d<T> const& p2,
             vgl_homg_point_2d<T> const& p3)
{ return (p3-p1)/(p2-p1); }

//: Return the point at a given ratio wrt two other points.
//  By default, the mid point (ratio=0.5) is returned.
//  Note that the third argument is T, not double, so the midpoint of e.g.
//  two vgl_homg_point_2d<int> is not a valid concept.  But the reflection point
//  of p2 wrt p1 is: in that case f=-1.
template <class T> inline
vgl_homg_point_2d<T> midpoint(vgl_homg_point_2d<T> const& p1,
                              vgl_homg_point_2d<T> const& p2,
                              T f = (T)0.5)
{ return p1 + f*(p2-p1); }


//: Return the point at the centre of gravity of two given points.
// Identical to midpoint(p1,p2).
// Invalid when both points are at infinity.
// If only one point is at infinity, that point is returned.
// \relates vgl_homg_point_2d
template <class T> inline
vgl_homg_point_2d<T> centre(vgl_homg_point_2d<T> const& p1,
                            vgl_homg_point_2d<T> const& p2)
{
  return vgl_homg_point_2d<T>(p1.x()*p2.w() + p2.x()*p1.w(),
                              p1.y()*p2.w() + p2.y()*p1.w(),
                              p1.w()*p2.w()*2 );
}

//: Return the point at the centre of gravity of a set of given points.
// There are no rounding errors when T is e.g. int, if all w() are 1.
// \relates vgl_homg_point_2d
template <class T> inline
vgl_homg_point_2d<T> centre(vcl_vector<vgl_homg_point_2d<T> > const& v)
{
  int n=v.size();
  assert(n>0); // it is *not* correct to return the point (0,0) when n==0.
  T x = 0, y = 0;
  for (int i=0; i<n; ++i) x+=v[i].x()/v[i].w(), y+=v[i].y()/v[i].w();
  return vgl_homg_point_2d<T>(x,y,(T)n);
}

#define VGL_HOMG_POINT_2D_INSTANTIATE(T) extern "please include vgl/vgl_homg_point_2d.txx first"

#endif // vgl_homg_point_2d_h_
