// This is core/vgl/vgl_homg_point_3d.h
#ifndef vgl_homg_point_3d_h_
#define vgl_homg_point_3d_h_
//:
// \file
// \brief point in projective 3D space
// \author Don HAMILTON, Peter TU
//
// \verbatim
//  Modifications
//   Peter Vanroose -  4 July 2001 - Added geometric interface like vgl_point_3d
//   Peter Vanroose -  2 July 2001 - Added constructor from 3 planes
//   Peter Vanroose -  1 July 2001 - Renamed data to x_ y_ z_ w_ and inlined constructors
//   Peter Vanroose - 27 June 2001 - Implemented operator==
//   Peter Vanroose - 15 July 2002 - Added coplanar()
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_plane_3d
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

//: Represents a homogeneous 3D point
template <class Type>
class vgl_homg_point_3d
{
  // the data associated with this point
  Type x_;
  Type y_;
  Type z_;
  Type w_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Default constructor with (0,0,0,1)
  inline vgl_homg_point_3d() : x_(0), y_(0), z_(0), w_((Type)1) {}

  //: Construct from three (nonhomogeneous) or four (homogeneous) Types.
  inline vgl_homg_point_3d(Type px, Type py, Type pz, Type pw = (Type)1)
    : x_(px), y_(py), z_(pz), w_(pw) {}

  //: Construct from homogeneous 4-array.
  inline vgl_homg_point_3d(const Type v[4]) : x_(v[0]), y_(v[1]), z_(v[2]), w_(v[3]) {}

  //: Construct point at infinity from direction vector.
  inline vgl_homg_point_3d(vgl_vector_3d<Type>const& v) : x_(v.x()), y_(v.y()), z_(v.z()), w_(0) {}

  //: Construct from (non-homogeneous) vgl_point_3d<Type>
  inline explicit vgl_homg_point_3d(vgl_point_3d<Type> const& p)
    : x_(p.x()), y_(p.y()), z_(p.z()), w_((Type)1) {}

  //: Construct from 3 planes (intersection).
  vgl_homg_point_3d(vgl_homg_plane_3d<Type> const& l1,
                    vgl_homg_plane_3d<Type> const& l2,
                    vgl_homg_plane_3d<Type> const& l3);

#if 0
  // Default copy constructor
  inline vgl_homg_point_3d(const vgl_homg_point_3d<Type>& that)
    : x_(that.x()), y_(that.y()), z_(that.z()), w_(that.w()) {}

  // Destructor
  inline ~vgl_homg_point_3d() {}

  // Default assignment operator
  inline vgl_homg_point_3d<Type>& operator=(const vgl_homg_point_3d<Type>& that)
  {
    set(that.x(),that.y(),that.z(),that.w());
    return *this;
  }
#endif

  //: the comparison operator
  bool operator==(vgl_homg_point_3d<Type> const& other) const;
  inline bool operator!=(vgl_homg_point_3d<Type>const& other)const{return !operator==(other);}

  // Data Access-------------------------------------------------------------

  inline Type x() const { return x_; }
  inline Type y() const { return y_; }
  inline Type z() const { return z_; }
  inline Type w() const { return w_; }

  //: Set \a x,y,z,w
  // Note that it does not make sense to set \a x, \a y, \a z or \a w individually.
  inline void set(Type px, Type py, Type pz, Type pw = (Type)1)
  { x_=px; y_=py; z_=pz; w_=pw; }

  inline void set(Type const p[4]) { x_=p[0]; y_=p[1]; z_=p[2]; w_=p[3]; }

  //: Return true iff the point is at infinity (an ideal point).
  // The method checks whether |w| <= tol * max(|x|,|y|,|z|)
  inline bool ideal(Type tol = (Type)0) const
  {
#define vgl_Abs(x) (x<0?-x:x) // avoid #include of vcl_cmath.h AND vcl_cstdlib.h
    return vgl_Abs(w()) <= tol * vgl_Abs(x()) ||
           vgl_Abs(w()) <= tol * vgl_Abs(y()) ||
           vgl_Abs(w()) <= tol * vgl_Abs(z());
#undef vgl_Abs
  }

  inline bool get_nonhomogeneous(double& vx, double& vy, double& vz) const
  {
    if (w() == 0)
    {
#ifdef DEBUG
      vcl_cerr << "vgl_homg_point_3d::get_nonhomogeneous - point at infinity\n";
#endif
      return false;
    }

    double hw = 1.0/w();

    vx = x() * hw;
    vy = y() * hw;
    vz = z() * hw;

    return true;
  }

  inline bool rescale_w(Type new_w = Type(1))
  {
    if (w() == 0)
      return false;

    x_ = x() * new_w / w();
    y_ = y() * new_w / w();
    z_ = z() * new_w / w();
    w_ = new_w;

    return true;
  }
};

//  +-+-+ point_3d simple I/O +-+-+

//: Write "<vgl_homg_point_3d (x,y,z,w) >" to stream
// \relates vgl_homg_point_3d
template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_homg_point_3d<Type> const& p);

//: Read x y z w from stream
// \relates vgl_homg_point_3d
template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_homg_point_3d<Type>& p);

//  +-+-+ homg_point_3d arithmetic +-+-+

//: Return true iff the point is at infinity (an ideal point).
// The method checks whether |w| <= tol * max(|x|,|y|,|z|)
// \relates vgl_homg_point_3d
template <class Type> inline
bool is_ideal(vgl_homg_point_3d<Type> const& p, Type tol = (Type)0) { return p.ideal(tol); }

//: Return true iff the 4 points are coplanar, i.e., they belong to a common plane
// \relates vgl_homg_point_3d
template <class Type> inline
bool coplanar(vgl_homg_point_3d<Type> const& p1,
              vgl_homg_point_3d<Type> const& p2,
              vgl_homg_point_3d<Type> const& p3,
              vgl_homg_point_3d<Type> const& p4)
{
  return ((p1.x()*p2.y()-p1.y()*p2.x())*p3.z()
         +(p3.x()*p1.y()-p3.y()*p1.x())*p2.z()
         +(p2.x()*p3.y()-p2.y()*p3.x())*p1.z())*p4.w()
        +((p4.x()*p1.y()-p4.y()*p1.x())*p2.z()
         +(p2.x()*p4.y()-p2.y()*p4.x())*p1.z()
         +(p1.x()*p2.y()-p1.y()*p2.x())*p4.z())*p3.w()
        +((p3.x()*p4.y()-p3.y()*p4.x())*p1.z()
         +(p1.x()*p3.y()-p1.y()*p3.x())*p4.z()
         +(p4.x()*p1.y()-p4.y()*p1.x())*p3.z())*p2.w()
        +((p2.x()*p3.y()-p2.y()*p3.x())*p4.z()
         +(p4.x()*p2.y()-p4.y()*p2.x())*p3.z()
         +(p3.x()*p4.y()-p3.y()*p4.x())*p2.z())*p1.w() == 0;
}

//: The difference of two points is the vector from second to first point
// This function is only valid if the points are not at infinity.
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_vector_3d<Type> operator-(vgl_homg_point_3d<Type> const& p1,
                              vgl_homg_point_3d<Type> const& p2)
{
  assert(p1.w() && p2.w());
  return vgl_vector_3d<Type>(p1.x()/p1.w()-p2.x()/p2.w(),
                             p1.y()/p1.w()-p2.y()/p2.w(),
                             p1.z()/p1.w()-p2.z()/p2.w());
}

//: Adding a vector to a point gives a new point at the end of that vector
// If the point is at infinity, nothing happens.
// Note that vector + point is not defined!  It's always point + vector.
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type> operator+(vgl_homg_point_3d<Type> const& p,
                                  vgl_vector_3d<Type> const& v)
{
  return vgl_homg_point_3d<Type>(p.x()+v.x()*p.w(),
                                 p.y()+v.y()*p.w(),
                                 p.z()+v.z()*p.w(), p.w());
}

//: Adding a vector to a point gives the point at the end of that vector
// If the point is at infinity, nothing happens.
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type>& operator+=(vgl_homg_point_3d<Type>& p,
                                    vgl_vector_3d<Type> const& v)
{
  p.set(p.x()+v.x()*p.w(),p.y()+v.y()*p.w(),p.z()+v.z()*p.w(),p.w()); return p;
}

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type> operator-(vgl_homg_point_3d<Type> const& p,
                                  vgl_vector_3d<Type> const& v)
{ return p + (-v); }

//: Subtracting a vector from a point is the same as adding the inverse vector
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type>& operator-=(vgl_homg_point_3d<Type>& p,
                                    vgl_vector_3d<Type> const& v)
{ return p += (-v); }

//  +-+-+ homg_point_3d geometry +-+-+

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
// \relates vgl_homg_point_3d
template <class T>
double cross_ratio(vgl_homg_point_3d<T>const& p1, vgl_homg_point_3d<T>const& p2,
                   vgl_homg_point_3d<T>const& p3, vgl_homg_point_3d<T>const& p4);

//: Are three points collinear, i.e., do they lie on a common line?
// \relates vgl_homg_point_3d
template <class Type>
bool collinear(vgl_homg_point_3d<Type> const& p1,
               vgl_homg_point_3d<Type> const& p2,
               vgl_homg_point_3d<Type> const& p3);

//: Return the relative distance to p1 wrt p1-p2 of p3.
//  The three points should be collinear and p2 should not equal p1.
//  This is the coordinate of p3 in the affine 1D reference frame (p1,p2).
//  If p3=p1, the ratio is 0; if p1=p3, the ratio is 1.
//  The mid point of p1 and p2 has ratio 0.5.
//  Note that the return type is double, not Type, since the ratio of e.g.
//  two vgl_vector_3d<int> need not be an int.
// \relates vgl_homg_point_3d
template <class Type> inline
double ratio(vgl_homg_point_3d<Type> const& p1,
             vgl_homg_point_3d<Type> const& p2,
             vgl_homg_point_3d<Type> const& p3)
{ return (p3-p1)/(p2-p1); }

//: Return the point at a given ratio wrt two other points.
//  By default, the mid point (ratio=0.5) is returned.
//  Note that the third argument is Type, not double, so the midpoint of e.g.
//  two vgl_homg_point_3d<int> is not a valid concept.  But the reflection point
//  of p2 wrt p1 is: in that case f=-1.
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type> midpoint(vgl_homg_point_3d<Type> const& p1,
                                 vgl_homg_point_3d<Type> const& p2,
                                 Type f = (Type)0.5)
{ return p1 + f*(p2-p1); }


//: Return the point at the centre of gravity of two given points.
// Identical to midpoint(p1,p2).
// Invalid when both points are at infinity.
// If only one point is at infinity, that point is returned. inline
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type> centre(vgl_homg_point_3d<Type> const& p1,
                               vgl_homg_point_3d<Type> const& p2)
{
  return vgl_homg_point_3d<Type>(p1.x()*p2.w() + p2.x()*p1.w() ,
                                 p1.y()*p2.w() + p2.y()*p1.w() ,
                                 p1.z()*p2.w() + p2.z()*p1.w() ,
                                 p1.w()*p2.w()*2 );
}

//: Return the point at the centre of gravity of a set of given points.
// There are no rounding errors when Type is e.g. int, if all w() are 1.
// \relates vgl_homg_point_3d
template <class Type> inline
vgl_homg_point_3d<Type> centre(vcl_vector<vgl_homg_point_3d<Type> > const& v)
{
  int n=v.size();
  assert(n>0); // it is *not* correct to return the point (0,0) when n==0.
  Type x = 0, y = 0, z = 0;
  for (int i=0; i<n; ++i)
    x+=v[i].x()/v[i].w(), y+=v[i].y()/v[i].w(), z+=v[i].z()/v[i].w();
  return vgl_homg_point_3d<Type>(x,y,z,(Type)n);
}

#define VGL_HOMG_POINT_3D_INSTANTIATE(T) extern "please include vgl/vgl_homg_point_3d.txx first"

#endif // vgl_homg_point_3d_h_
