#ifndef vgl_homg_plane_3d_h
#define vgl_homg_plane_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_homg_plane_3d.h

//:
// \file
// \brief homogeneous plane in 3D projective space
// \author Don HAMILTON Peter TU
//
// \verbatim
// Modifications
// Peter Vanroose  6 July 2001: Now using vgl_vector_3d for normal direction
// Peter Vanroose  6 July 2001: Added normal(); replaced data_[4] by a_ b_ c_ d_
// Peter Vanroose  6 July 2001: Added constructor from 3 points
// CJB (Manchester) 16/03/2001: Tidied up the documentation
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_plane_3d and vgl_homg_point_3d
#include <vgl/vgl_vector_3d.h>
#include <vcl_cassert.h>

//: Represents a homogeneous 3D plane
template <class Type>
class vgl_homg_plane_3d {
public:
  vgl_homg_plane_3d () {}

  //: Construct from four Types.
  vgl_homg_plane_3d (Type nx, Type ny, Type nz, Type d) : a_(nx), b_(ny), c_(nz), d_(d) {}

  //: Construct from 4-vector.
  vgl_homg_plane_3d (const Type v[4]) : a_(v[0]), b_(v[1]), c_(v[2]), d_(v[3]) {}

  //: Construct from non-homogeneous plane.
  vgl_homg_plane_3d (vgl_plane_3d<Type> const& pl);

  //: Construct from normal and a point
  //  The given point must not be at infinity.
  vgl_homg_plane_3d (vgl_vector_3d<Type> const& n, vgl_homg_point_3d<Type> const& p);

  //: Construct from three non-collinear points
  vgl_homg_plane_3d (vgl_homg_point_3d<Type> const& p1,
                     vgl_homg_point_3d<Type> const& p2,
                     vgl_homg_point_3d<Type> const& p3);

  // Data Access-------------------------------------------------------------

  //: Return x coefficient
  inline Type a() const {return a_;}
  inline Type nx() const {return a_;}
  //: Return y coefficient
  inline Type b() const {return b_;}
  inline Type ny() const {return b_;}
  //: Return z coefficient
  inline Type c() const {return c_;}
  inline Type nz() const {return c_;}
  //: Return homogenous scaling coefficient
  inline Type d() const {return d_;}

  //: Set equation a*x+b*y+c*z+d*w=0
  inline void set(Type a,Type b,Type c,Type d){assert(a||b||c||d);a_=a;b_=b;c_=c;d_=d;}

  //: the equality operator
  bool operator==( vgl_homg_plane_3d<Type> const & pl) const;
  bool operator!=( vgl_homg_plane_3d<Type> const & pl) const { return !operator==(pl); }

  //: Return true iff the plane is the plane at infinity.
  // The method checks that max(|nx|,|ny|,|nz|) <= tol * |d|
  bool ideal(Type tol = Type(0)) const {
#define vgl_Abs(x) (x<0?-x:x) // avoid #include of vcl_cmath.h AND vcl_cstdlib.h
    return vgl_Abs(nx()) <= tol * vgl_Abs(d()) &&
           vgl_Abs(ny()) <= tol * vgl_Abs(d()) &&
           vgl_Abs(nz()) <= tol * vgl_Abs(d());
#undef vgl_Abs
  }

  inline vgl_vector_3d<double> normal() const { return normalized(vgl_vector_3d<double>(a(),b(),c())); }

private:
  // the four homogenenous coordinates of the point.
  Type a_;
  Type b_;
  Type c_;
  Type d_;
};

//: Return true iff p is the plane at infinity
template <class Type>
bool is_ideal(vgl_homg_plane_3d<Type> const& p, Type tol = Type(0)) { return p.ideal(tol); }

// stream operators

template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_plane_3d<Type>& p);

template <class Type>
vcl_istream&  operator>>(vcl_istream& is, vgl_homg_plane_3d<Type>& p);

#define VGL_HOMG_PLANE_3D_INSTANTIATE(T) extern "please include vgl/vgl_homg_plane_3d.txx first"

#endif // vgl_homg_plane_3d_h
