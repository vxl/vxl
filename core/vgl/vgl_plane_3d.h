// This is core/vgl/vgl_plane_3d.h
#ifndef vgl_plane_3d_h
#define vgl_plane_3d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a plane in 3D nonhomogeneous space
// \author Don Hamilton, Peter Tu
// \date   Feb 15 2000
//
// \verbatim
//  Modifications
//   Peter Vanroose  6 July 2001: Added assertion in constructors
//   Peter Vanroose  6 July 2001: Now using vgl_vector_3d for normal direction
//   Peter Vanroose  6 July 2001: Implemented constructor from 3 points
//   Peter Vanroose  6 July 2001: Added normal(); replaced data_[4] by a_ b_ c_ d_
//   Peter Vanroose  6 July 2001: Added operator== and operator!=
//   Peter Vanroose 19 Aug. 2004: implementation of both constructors corrected
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_plane_3d, vgl_point_3d
#include <vgl/vgl_vector_3d.h>

//: Represents a Euclidean 3D plane
//  The equation of the plane is $ a x + b y + c z + d = 0 $
template <class T>
class vgl_plane_3d
{
  // the data associated with this plane
  T a_;
  T b_;
  T c_;
  T d_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  // Default constructor: horizontal XY-plane (equation 1.z = 0)
  inline vgl_plane_3d () : a_(0), b_(0), c_(1), d_(0) {}

#if 0
  // Default copy constructor - compiler provides the correct one
  inline vgl_plane_3d (vgl_plane_3d<T> const& pl)
    : a_(pl.a()), b_(pl.b()), c_(pl.c()), d_(pl.d()) {}
  // Default destructor - compiler provides the correct one
  inline ~vgl_plane_3d () {}
  // Default assignment operator - compiler provides the correct one
  inline vgl_plane_3d<T>& operator=(vgl_plane_3d<T> const& pl)
  { a_ = pl.a(); b_ = pl.b(); c_ = pl.c(); d_ = pl.d(); return *this; }
#endif

  //: Construct a vgl_plane_3d from its equation $ax+by+cz+d=0$
  //  At least one of a, b or c should be nonzero.
  inline vgl_plane_3d (T a,T b,T c,T d)
    : a_(a), b_(b), c_(c), d_(d) { assert(a||b||c); }

  //: Construct a vgl_plane_3d from its equation $v[0]x+v[1]y+v[2]z+v[3]=0$
  //  At least one of v[0], v[1] or v[2] should be nonzero.
  inline vgl_plane_3d (const T v[4])
    : a_(v[0]), b_(v[1]), c_(v[2]), d_(v[3]) { assert(a_||b_||c_); }

  //: Construct from a homogeneous plane
  vgl_plane_3d (vgl_homg_plane_3d<T> const& p);

  //: Construct from Normal and a point
  //  The plane goes through the point \a p and will be orthogonal to \a normal.
  vgl_plane_3d (vgl_vector_3d<T> const& normal,
                vgl_point_3d<T> const& p);

  //: Construct from three non-collinear points
  //  The plane will contain all three points \a p1, \a p2 and \a p3.
  vgl_plane_3d (vgl_point_3d<T> const& p1,
                vgl_point_3d<T> const& p2,
                vgl_point_3d<T> const& p3);

  // Data Access-------------------------------------------------------------

  //: Return \a x coefficient
  inline T a()  const {return a_;}
  inline T nx() const {return a_;}
  //: Return \a y coefficient
  inline T b()  const {return b_;}
  inline T ny() const {return b_;}
  //: Return \a z coefficient
  inline T c()  const {return c_;}
  inline T nz() const {return c_;}
  //: Return constant coefficient
  inline T d()  const {return d_;}

  //: Set this vgl_plane_3d to have the equation $ax+by+cz+d=0$
  inline void set(T a,T b,T c,T d) { assert(a||b||c); a_=a; b_=b; c_=c; d_=d; }

  //: the comparison operator
  //  The equations need not be identical, but just equivalent.
  bool operator==( vgl_plane_3d<T> const& p) const;
  inline bool operator!=( vgl_plane_3d<T>const& p)const{return !operator==(p);}

  //: Return true iff the plane is the plane at infinity.
  //  Always returns false
  inline bool ideal(T = (T)0) const { return false; }

  //: Return the normal direction, i.e., a unit vector orthogonal to this plane
  inline vgl_vector_3d<T> normal() const
  { return normalized(vgl_vector_3d<T>(a(),b(),c())); }
};

//: Return true iff p is the plane at infinity
//  Always returns false
template <class T> inline
bool is_ideal(vgl_plane_3d<T> const&, T=(T)0) { return false; }

//: Write to stream
// \relates vgl_plane_3d
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vgl_plane_3d<T>& p);

//: Read from stream
// \relates vgl_plane_3d
template <class T>
vcl_istream& operator>>(vcl_istream& is, vgl_plane_3d<T>& p);

#define VGL_PLANE_3D_INSTANTIATE(T) extern "please include vgl/vgl_plane_3d.txx first"

#endif // vgl_plane_3d_h
