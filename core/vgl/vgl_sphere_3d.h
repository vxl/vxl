// This is core/vgl/vgl_sphere_3d.h
#ifndef vgl_sphere_3d_h
#define vgl_sphere_3d_h
//:
// \file
// \brief a sphere in 3D nonhomogeneous space
// \author Ian Scott

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h> // forward declare vgl_line_3d_2_points
#include <vgl/vgl_point_3d.h>

//: Represents a cartesian 3D point
template <class Type>
class vgl_sphere_3d
{
  vgl_point_3d<Type> c_; //!< centre
  Type r_;               //!< radius
 public:

  // Constructors/Initializers/Destructor------------------------------------

  //: Default constructor
  inline vgl_sphere_3d (): c_(Type(0), Type(0), Type(0)), r_(Type(-1)) {}

  //: Construct from four scalars: centre and radius.
  inline vgl_sphere_3d(Type px, Type py, Type pz, Type rad) : c_(px, py, pz), r_(rad) {}

  //: Construct from a 4-array, representing centre and radius.
  inline vgl_sphere_3d (const Type v[4]): c_(v[0], v[1], v[2]), r_(v[3]) {}

  //: Construct from centre point and radius.
  vgl_sphere_3d (vgl_point_3d<Type> const& cntr, Type rad): c_(cntr), r_(rad) {}

  //: Test for equality
  inline bool operator==(const vgl_sphere_3d<Type> &s) const { return this==&s || (c_==s.c_ && r_==s.r_); }
  //: Test for inequality
  inline bool operator!=(vgl_sphere_3d<Type>const& s) const { return !operator==(s); }

  // Data Access-------------------------------------------------------------

  inline const vgl_point_3d<Type> & centre() const {return c_;}
  inline Type radius() const {return r_;}

  //: Return true if this sphere is empty
  inline bool is_empty() const {
    return r_ < 0.0;
  }

  //: Return true iff the point p is inside (or on) this sphere
  bool contains(vgl_point_3d<Type> const& p) const;

  //: Make the sphere empty.
  void set_empty() {c_.set(0,0,0); r_=-1;}

  //: Set radius \a r of this sphere (while centre unchanged)
  inline void set_radius(Type r) { r_=r; }
  //: Set centre of this sphere to \a c (while radius unchanged)
  inline void set_centre(const vgl_point_3d<Type> & c) { c_=c; }

  //: Calculate the end points of a line clipped by this sphere.
  bool clip(const vgl_line_3d_2_points<Type> & line,
            vgl_point_3d<Type> &p1, vgl_point_3d<Type> &p2) const;


  //: convert point on sphere to Cartesian coordinates, angles in radians
  void spherical_to_cartesian(Type elevation_rad, Type azimuth_rad,
                              Type& x, Type& y, Type& z) const;

  void spherical_to_cartesian(Type elevation_rad, Type azimuth_rad,
                              vgl_point_3d<Type>&  pt) const;

  //:find elevation and azimuth of closest point on the sphere to x,y,z
  void cartesian_to_spherical(Type x, Type y, Type z, Type& elevation_rad, Type& azimuth_rad) const;
  void cartesian_to_spherical(vgl_point_3d<Type> const& pt, Type& elevation_rad, Type& azimuth_rad) const;

  //: Writes "<vgl_sphere_3d centre=vgl_point_3d<x,y,z> radius=r)>" to stream
  std::ostream& print(std::ostream& os) const;


  //: Read from stream, possibly with formatting.
  //  Either just reads 4 blank-separated numbers,
  //  or reads 4 comma-separated numbers,
  //  or reads 4 numbers in parenthesized form "(123, 321, 567, 890)"
  std::istream& read(std::istream& is);
};


//: Writes "<vgl_sphere_3d centre=vgl_point_3d<x,y,z> radius=r)>" to stream
template <class Type>
std::ostream& operator<<(std::ostream& os, const vgl_sphere_3d<Type>& sph);

//: Read from stream, possibly with formatting.
//  Either just reads 4 blank-separated numbers,
//  or reads 4 comma-separated numbers,
//  or reads 4 numbers in parenthesized form "(123, 321, 567, 890)"
template <class Type>
std::istream& operator>>(std::istream& is, vgl_sphere_3d<Type>& sph);


#define VGL_SPHERE_3D_INSTANTIATE(T) extern "please include vgl/vgl_sphere_3d.hxx first"

#endif // vgl_sphere_3d_h
