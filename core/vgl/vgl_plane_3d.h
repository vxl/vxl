// This is core/vgl/vgl_plane_3d.h
#ifndef vgl_plane_3d_h
#define vgl_plane_3d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief a plane in 3D nonhomogeneous space
//  \author Don Hamilton, Peter Tu
//  \date   Feb 15 2000
//
// \verbatim
//  Modifications
//   Peter Vanroose  6 July 2001: Added assertion in constructors
//   Peter Vanroose  6 July 2001: Now using vgl_vector_3d for normal direction
//   Peter Vanroose  6 July 2001: Implemented constructor from 3 points
//   Peter Vanroose  6 July 2001: Added normal(); replaced data_[4] by a_ b_ c_ d_
//   Peter Vanroose  6 July 2001: Added operator== and operator!=
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_plane_3d, vgl_point_3d
#include <vgl/vgl_vector_3d.h>

//: Represents a Euclidean 3D plane
//  the equation of the plane is (nx * x) + (ny * y) + (nz * z) + d = 0
template <class Type>
class vgl_plane_3d
{
  // the data associated with this plane
  Type a_;
  Type b_;
  Type c_;
  Type d_;

 public:

  // Constructors/Initializers/Destructor------------------------------------

  // Default constructor: XY-plane (1.z = 0)
  inline vgl_plane_3d () : a_(0), b_(0), c_(1), d_(0) {}

#if 0
  // Default copy constructor
  inline vgl_plane_3d (vgl_plane_3d<Type> const& pl)
    : a_(pl.a()), b_(pl.b()), c_(pl.c()), d_(pl.d()) {}
#endif

  //: Construct a vgl_plane_3d from its equation, four Types.
  inline vgl_plane_3d (Type a,Type b,Type c,Type d)
    : a_(a), b_(b), c_(c), d_(d) { assert(a||b||c); }

  //: Construct from its equation, a 4-vector.
  inline vgl_plane_3d (const Type v[4])
    : a_(v[0]), b_(v[1]), c_(v[2]), d_(v[3]) { assert(a_||b_||c_); }

  //: Construct from a homogeneous plane
  vgl_plane_3d (vgl_homg_plane_3d<Type> const& p);

  //: Construct from Normal and a point
  vgl_plane_3d (vgl_vector_3d<Type> const& normal,
                vgl_point_3d<Type> const& p);

  //: Construct from three non-collinear points
  vgl_plane_3d (vgl_point_3d<Type> const& p1,
                vgl_point_3d<Type> const& p2,
                vgl_point_3d<Type> const& p3);

#if 0
  // Default destructor
  inline ~vgl_plane_3d () {}

  // Default assignment operator
  inline vgl_plane_3d<Type>& operator=(vgl_plane_3d<Type> const& pl)
  { a_ = pl.a(); b_ = pl.b(); c_ = pl.c(); d_ = pl.d(); return *this; }
#endif

  // Data Access-------------------------------------------------------------

  //: Return \a x coefficient
  inline Type a()  const {return a_;}
  inline Type nx() const {return a_;}
  //: Return \a y coefficient
  inline Type b()  const {return b_;}
  inline Type ny() const {return b_;}
  //: Return \a z coefficient
  inline Type c()  const {return c_;}
  inline Type nz() const {return c_;}
  //: Return constant coefficient
  inline Type d()  const {return d_;}

  //: Set equation \a a*x+b*y+c*z+d=0
  inline void set(Type a,Type b,Type c,Type d){assert(a||b||c);a_=a;b_=b;c_=c;d_=d;}

  //: the comparison operator
  bool operator==( vgl_plane_3d<Type> const& p) const;
  inline bool operator!=( vgl_plane_3d<Type>const& p)const{return !operator==(p);}

  //: Return true iff the plane is the plane at infinity.
  //  Always returns false
  inline bool ideal(Type = (Type)0) const { return false; }

  inline vgl_vector_3d<Type> normal() const
  { return normalized(vgl_vector_3d<Type>(a(),b(),c())); }
};


//: Return true iff p is the plane at infinity
//  Always returns false
template <class Type> inline
bool is_ideal(vgl_plane_3d<Type> const&, Type=(Type)0) {return false;}

//: Write to stream
// \relates vgl_plane_3d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_plane_3d<Type>& p);

//: Read from stream
// \relates vgl_plane_3d
template <class Type>
vcl_istream&  operator>>(vcl_istream& is, vgl_plane_3d<Type>& p);

#define VGL_PLANE_3D_INSTANTIATE(T) extern "please include vgl/vgl_plane_3d.txx first"

#endif // vgl_plane_3d_h
