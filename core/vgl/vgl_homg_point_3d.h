#ifndef vgl_homg_point_3d_h_
#define vgl_homg_point_3d_h_

// This is vxl/vgl/vgl_homg_point_3d.h

//:
// \file
// \author Don HAMILTON, Peter TU
//
// \verbatim
// Modifications
// Peter Vanroose -  2 July 2001 - Added constructor from 3 planes
// Peter Vanroose -  1 July 2001 - Renamed data to x_ y_ z_ w_ and inlined constructors
// Peter Vanroose - 27 June 2001 - Implemented operator==
// \endverbatim

#include <vcl_cmath.h> // for vcl_abs(double) etc
#include <vcl_cstdlib.h> // for vcl_abs(int) etc
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_plane_3d
#include <vcl_iosfwd.h>

//: Represents a homogenious 3D point
template <class Type>
class vgl_homg_point_3d
{
  //***************************************************************************
  // Initialization
  //***************************************************************************
public:

  //: Default constructor with (0,0,0,1)
  vgl_homg_point_3d(void) : x_(0), y_(0), z_(0), w_(Type(1)) {}

  explicit vgl_homg_point_3d(vgl_point_3d<Type> const& p)
    : x_(p.x()), y_(p.y()), z_(p.z()), w_(Type(1)) {}

  //: Constructor from three or four Types
  vgl_homg_point_3d(Type px, Type py, Type pz, Type pw = Type(1))
    : x_(px), y_(py), z_(pz), w_(pw) {}

  //: Construct from 4-vector.
  vgl_homg_point_3d(const Type v[4])
    : x_(v[0]), y_(v[1]), z_(v[2]), w_(v[3]) {}

  //: Construct from 3 planes (intersection).
  vgl_homg_point_3d(vgl_homg_plane_3d<Type> const& l1,
                    vgl_homg_plane_3d<Type> const& l2,
                    vgl_homg_plane_3d<Type> const& l3);

#if 0
  // Default copy constructor
  vgl_homg_point_3d (const vgl_homg_point_3d<Type>& that)
    : x_(that.x()), y_(that.y()), z_(that.z()), w_(that.w()) {}

  // Destructor
  ~vgl_homg_point_3d () {}

  // Default assignment operator
  vgl_homg_point_3d<Type>& operator=(const vgl_homg_point_3d<Type>& that) {
    set(that.x(),that.y(),that.z(),that.w());
    return *this;
  }
#endif

  //***************************************************************************
  // Data Access
  //***************************************************************************

  inline Type x() const { return x_; }
  inline Type y() const { return y_; }
  inline Type z() const { return z_; }
  inline Type w() const { return w_; }

  //: Set x,y,z,w
  inline void set(Type px, Type py, Type pz, Type pw = (Type)1)
  { x_=px; y_=py; z_=pz; w_=pw; }

  //: the equality operator
  bool operator==(vgl_homg_point_3d<Type> const& other) const;
  bool operator!=(vgl_homg_point_3d<Type> const& other) const { return ! operator==(other); }

  //: Test for point at infinity
  // Return true when |w| < tol * max(|x|, |y|, |z|)
  bool ideal(Type tol = Type(0)) {
    return vcl_abs(w()) <= tol * vcl_abs(x()) ||
           vcl_abs(w()) <= tol * vcl_abs(y()) ||
           vcl_abs(w()) <= tol * vcl_abs(z());
  }

  //***************************************************************************
  // Internals
  //***************************************************************************

private:
  // the data associated with this point
  Type x_;
  Type y_;
  Type z_;
  Type w_;
};

//*****************************************************************************
// Stream operators
//*****************************************************************************

template <class Type>
vcl_ostream &operator<<(vcl_ostream &s,
                    const vgl_homg_point_3d<Type> &p);

template <class Type>
vcl_istream &operator>>(vcl_istream &is,
                    vgl_homg_point_3d<Type> &p);

#endif // vgl_homg_point_3d_h_
