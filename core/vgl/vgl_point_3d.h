// This is core/vgl/vgl_point_3d.h
#ifndef vgl_point_3d_h
#define vgl_point_3d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a point in 3D nonhomogeneous space
// \author Don Hamilton, Peter Tu
//
// \verbatim
//  Modifications
//   Peter Vanroose -  2 July 2001 - Added constructor from 3 planes
//   Peter Vanroose - 24 Oct. 2002 - Added coplanar()
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_plane_3d
#include <vgl/vgl_vector_3d.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

//: Represents a cartesian 3D point
template <class Type>
class vgl_point_3d
{
  // the data associated with this point
  Type x_;
  Type y_;
  Type z_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Default constructor
  inline vgl_point_3d () {}

  //: Construct from three Types.
  inline vgl_point_3d(Type px, Type py, Type pz) : x_(px), y_(py), z_(pz) {}

  //: Construct from 3-array.
  inline vgl_point_3d (const Type v[3]) : x_(v[0]), y_(v[1]), z_(v[2]) {}

  //: Construct from homogeneous point
  vgl_point_3d (vgl_homg_point_3d<Type> const& p);

  //: Construct from 3 planes (intersection).
  vgl_point_3d (vgl_plane_3d<Type> const& pl1,
                vgl_plane_3d<Type> const& pl2,
                vgl_plane_3d<Type> const& pl3);

#if 0 // The compiler defaults for these are doing what they should do:
  //: Copy constructor
  inline vgl_point_3d(vgl_point_3d<Type> const& p) : x_(p.x()), y_(p.y()), z_(p.z()) {}
  //: Destructor
  inline ~vgl_point_3d () {}
  //: Assignment
  inline vgl_point_3d<Type>& operator=(vgl_point_3d<Type>const& p)
  { set(p.x(),p.y(),p.z()); return *this; }
#endif

  //: Test for equality
  inline bool operator==(const vgl_point_3d<Type> &p) const
  { return this==&p || (x_==p.x() && y_==p.y() && z_==p.z()); }
  inline bool operator!=(vgl_point_3d<Type>const& p)const
  { return !operator==(p); }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return x_;}
  inline Type y() const {return y_;}
  inline Type z() const {return z_;}

  //: Set \a x, \a y and \a z
  //  Note that \a x, \a y, or \a z cannot be set individually
  inline void set (Type px, Type py, Type pz){ x_ = px; y_ = py; z_ = pz; }
  //: Set \a x, \a y and \a z
  //  Note that \a x, \a y, or \a z cannot be set individually
  inline void set (Type const p[3]) { x_ = p[0]; y_ = p[1]; z_ = p[2]; }

  //: Return true iff the point is at infinity (an ideal point).
  //  Always returns false.
  inline bool ideal(Type = (Type)0) const { return false; }
};

//  +-+-+ point_3d simple I/O +-+-+

//: Write "<vgl_point_3d x,y,z> " to stream
// \relates vgl_point_3d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_point_3d<Type> const& p);

//: Read x y z from stream
// \relates vgl_point_3d
template <class Type>
vcl_istream&  operator>>(vcl_istream& s, vgl_point_3d<Type>& p);

//  +-+-+ point_3d arithmetic +-+-+

//: Return true iff the point is at infinity (an ideal point).
//  Always returns false.
template <class Type> inline
bool is_ideal(vgl_point_3d<Type> const&, Type = 0) { return false; }

//: The difference of two points is the vector from second to first point
// \relates vgl_point_3d
template <class Type> inline
vgl_vector_3d<Type> operator-(vgl_point_3d<Type> const& p1,
                              vgl_point_3d<Type> const& p2)
{ return vgl_vector_3d<Type>(p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z()); }

//: Adding a vector to a point gives a new point at the end of that vector
// Note that vector + point is not defined!  It's always point + vector.
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> operator+(vgl_point_3d<Type> const& p,
                             vgl_vector_3d<Type> const& v)
{ return vgl_point_3d<Type>(p.x()+v.x(), p.y()+v.y(), p.z()+v.z()); }

//: Adding a vector to a point gives the point at the end of that vector
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type>& operator+=(vgl_point_3d<Type>& p,
                               vgl_vector_3d<Type> const& v)
{ p.set(p.x()+v.x(), p.y()+v.y(), p.z()+v.z()); return p; }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> operator-(vgl_point_3d<Type> const& p,
                             vgl_vector_3d<Type> const& v)
{ return p + (-v); }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type>& operator-=(vgl_point_3d<Type>& p,
                               vgl_vector_3d<Type> const& v)
{ return p += (-v); }

//  +-+-+ point_3d geometry +-+-+

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
// \relates vgl_point_3d
template <class T>
double cross_ratio(vgl_point_3d<T>const& p1, vgl_point_3d<T>const& p2,
                   vgl_point_3d<T>const& p3, vgl_point_3d<T>const& p4);

//: Are three points collinear, i.e., do they lie on a common line?
// \relates vgl_point_3d
template <class Type> inline
bool collinear(vgl_point_3d<Type> const& p1,
               vgl_point_3d<Type> const& p2,
               vgl_point_3d<Type> const& p3)
{ return parallel(p1-p2, p1-p3); }

//: Return the relative distance to p1 wrt p1-p2 of p3.
//  The three points should be collinear and p2 should not equal p1.
//  This is the coordinate of p3 in the affine 1D reference frame (p1,p2).
//  If p3=p1, the ratio is 0; if p1=p3, the ratio is 1.
//  The mid point of p1 and p2 has ratio 0.5.
//  Note that the return type is double, not Type, since the ratio of e.g.
//  two vgl_vector_3d<int> need not be an int.
// \relates vgl_point_3d
template <class Type> inline
double ratio(vgl_point_3d<Type> const& p1,
             vgl_point_3d<Type> const& p2,
             vgl_point_3d<Type> const& p3)
{ return (p3-p1)/(p2-p1); }

//: Return the point at a given ratio wrt two other points.
//  By default, the mid point (ratio=0.5) is returned.
//  Note that the third argument is Type, not double, so the midpoint of e.g.
//  two vgl_point_3d<int> is not a valid concept.  But the reflection point
//  of p2 wrt p1 is: in that case f=-1.
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> midpoint(vgl_point_3d<Type> const& p1,
                            vgl_point_3d<Type> const& p2,
                            Type f = (Type)0.5)
{
  return vgl_point_3d<Type>((Type)((1-f)*p1.x() + f*p2.x()),
                            (Type)((1-f)*p1.y() + f*p2.y()),
                            (Type)((1-f)*p1.z() + f*p2.z()));
}


//: Return the point at the centre of gravity of two given points.
// Identical to midpoint(p1,p2).
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> centre(vgl_point_3d<Type> const& p1,
                          vgl_point_3d<Type> const& p2)
{
  return vgl_point_3d<Type>((p1.x() + p2.x())/2 ,
                            (p1.y() + p2.y())/2 ,
                            (p1.z() + p2.z())/2 );
}

//: Return the point at the centre of gravity of three given points.
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> centre(vgl_point_3d<Type> const& p1,
                          vgl_point_3d<Type> const& p2,
                          vgl_point_3d<Type> const& p3)
{
  return vgl_point_3d<Type>((p1.x() + p2.x() + p3.x())/3 ,
                            (p1.y() + p2.y() + p3.y())/3 ,
                            (p1.z() + p2.z() + p3.z())/3 );
}

//: Return the point at the centre of gravity of four given points.
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> centre(vgl_point_3d<Type> const& p1,
                          vgl_point_3d<Type> const& p2,
                          vgl_point_3d<Type> const& p3,
                          vgl_point_3d<Type> const& p4)
{
  return vgl_point_3d<Type>((p1.x() + p2.x() + p3.x() + p4.x())/4 ,
                            (p1.y() + p2.y() + p3.y() + p4.y())/4 ,
                            (p1.z() + p2.z() + p3.z() + p4.z())/4 );
}

//: Return the point at the centre of gravity of a set of given points.
// Beware of possible rounding errors when Type is e.g. int.
// \relates vgl_point_3d
template <class Type> inline
vgl_point_3d<Type> centre(vcl_vector<vgl_point_3d<Type> > const& v)
{
  int n=v.size();
  assert(n>0); // it is *not* correct to return the point (0,0) when n==0.
  Type x = 0, y = 0, z = 0;
  for (int i=0; i<n; ++i) x+=v[i].x(), y+=v[i].y(), z+=v[i].z();
  return vgl_point_3d<Type>(x/n,y/n,z/n);
}

//: Return true iff the 4 points are coplanar, i.e., they belong to a common plane
// \relates vgl_point_3d
template <class Type> inline
bool coplanar(vgl_point_3d<Type> const& p1,
              vgl_point_3d<Type> const& p2,
              vgl_point_3d<Type> const& p3,
              vgl_point_3d<Type> const& p4)
{
  return (p1.x()*p2.y()-p1.y()*p2.x())*p3.z()
        +(p3.x()*p1.y()-p3.y()*p1.x())*p2.z()
        +(p2.x()*p3.y()-p2.y()*p3.x())*p1.z()
        +(p4.x()*p1.y()-p4.y()*p1.x())*p2.z()
        +(p2.x()*p4.y()-p2.y()*p4.x())*p1.z()
        +(p1.x()*p2.y()-p1.y()*p2.x())*p4.z()
        +(p3.x()*p4.y()-p3.y()*p4.x())*p1.z()
        +(p1.x()*p3.y()-p1.y()*p3.x())*p4.z()
        +(p4.x()*p1.y()-p4.y()*p1.x())*p3.z()
        +(p2.x()*p3.y()-p2.y()*p3.x())*p4.z()
        +(p4.x()*p2.y()-p4.y()*p2.x())*p3.z()
        +(p3.x()*p4.y()-p3.y()*p4.x())*p2.z() == 0;
}

#define VGL_POINT_3D_INSTANTIATE(T) extern "please include vgl/vgl_point_3d.txx first"

#endif // vgl_point_3d_h
