#ifndef vgl_point_2d_h
#define vgl_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_point_2d.h

//:
// \file
// \brief a point in 2D nonhomogeneous space
// \author Don Hamilton, Peter Tu
// \verbatim
// Modifications :
//  2 July 2001 Peter Vanroose implemented constructor from homg point
// 29 June 2001 Peter Vanroose moved arithmetic operators to new vgl_vector_2d
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // declare vgl_homg_point_2d and vgl_line_2d
#include <vgl/vgl_vector_2d.h>

//: Represents a cartesian 2D point
template <class Type>
class vgl_point_2d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Construct from homogeneous point
  vgl_point_2d (vgl_homg_point_2d<Type> const& p);

  //: Construct a vgl_point_2d from two Types.
  vgl_point_2d (Type px, Type py) : x_(px), y_(py) {}

  //: Construct from 2-array.
  vgl_point_2d (Type const v[2]) : x_(v[0]), y_(v[1]) {}

  //: Construct from 2 lines (intersection).
  vgl_point_2d (vgl_line_2d<Type> const& l1,
                vgl_line_2d<Type> const& l2);

  //: Default constructor
  vgl_point_2d () {}

#if 0 // The compiler defaults for these are doing what they should do:
  //: Destructor
  ~vgl_point_2d () {}
  //: Copy constructor
  vgl_point_2d (vgl_point_2d<Type> const& p) x_(p.x()), y_(p.y()) {}
#endif

  //: Assignment
  inline vgl_point_2d<Type>& operator=(const vgl_point_2d<Type>& that){
    x_ = that.x(); y_ = that.y(); return *this;
  }

  // the == operator
  inline bool operator==(const vgl_point_2d<Type> &p) const {
    return this==&p || (x_==p.x() && y_==p.y());
  }
  inline bool operator!=(const vgl_point_2d<Type> &other) const { return ! operator==(other); }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return x_;}
  inline Type y() const {return y_;}

  //: Set x,y.
  inline void set (Type px, Type py){ x_ = px; y_ = py; }
  //: Set x,y.
  inline void set (Type const p[2]) { x_ = p[0]; y_ = p[1]; }
  inline void set_x (Type px) { x_ = px; }
  inline void set_y (Type py) { y_ = py; }

  // INTERNALS---------------------------------------------------------------

private:
  // the data associated with this point
  Type x_;
  Type y_;
};

//  +-+-+ point_2d simple I/O +-+-+

//: Write "<vgl_point_2d x,y>" to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream&, vgl_point_2d<Type> const& p);

//: Read x y from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream&, vgl_point_2d<Type>& p);

//  +-+-+ point_2d arithmetic +-+-+

//: The difference of two points is the vector from second to first point
template <class Type>
inline vgl_vector_2d<Type> operator-(vgl_point_2d<Type> const& p1,
                                     vgl_point_2d<Type> const& p2) {
  return vgl_vector_2d<Type>(p1.x()-p2.x(), p1.y()-p2.y());
}

//: Adding a vector to a point gives a new point at the end of that vector
// Note that vector + point is not defined!  It's always point + vector.
template <class Type>
inline vgl_point_2d<Type> operator+(vgl_point_2d<Type> const& p,
                                    vgl_vector_2d<Type> const& v) {
  return vgl_point_2d<Type>(p.x()+v.x(), p.y()+v.y());
}

//: Subtracting a vector from a point is the same as adding the inverse vector
template <class Type>
inline vgl_point_2d<Type> operator-(vgl_point_2d<Type> const& p,
                                    vgl_vector_2d<Type> const& v) {
  return p + (-v);
}

//  +-+-+ point_2d geometry +-+-+

//: Are three points collinear, i.e., do they lie on a common line?
template <class Type>
inline bool collinear(vgl_point_2d<Type> const& p1,
                      vgl_point_2d<Type> const& p2,
                      vgl_point_2d<Type> const& p3) {
  return parallel(p1-p2, p1-p3);
}

//: Return the relative distance to p1 wrt p1-p2 of p3.
//  The three points should be collinear and p2 should not equal p1.
//  This is the coordinate of p3 in the affine 1D reference frame (p1,p2).
//  If p3=p1, the ratio is 0; if p1=p3, the ratio is 1.
//  The mid point of p1 and p2 has ratio 0.5.
//  Note that the return type is double, not Type, since the ratio of e.g.
//  two vgl_vector_2d<int> need not be an int.
template <class Type>
inline double ratio(vgl_point_2d<Type> const& p1,
                    vgl_point_2d<Type> const& p2,
                    vgl_point_2d<Type> const& p3) {
  return (p3-p1)/(p2-p1);
}

//: Return the point at a given ratio wrt two other points.
//  By default, the mid point (ratio=0.5) is returned.
//  Note that the third argument is Type, not double, so the midpoint of e.g.
//  two vgl_point_2d<int> is not a valid concept.  But the reflection point
//  of p2 wrt p1 is: in that case f=-1.
template <class Type>
inline vgl_point_2d<Type> midpoint(vgl_point_2d<Type> const& p1,
                                   vgl_point_2d<Type> const& p2,
                                   Type f = 0.5) {
  return p1 + f*(p2-p1);
}

#endif // vgl_point_2d_h
