// This is core/vgl/algo/vgl_ellipsoid_3d.h
#ifndef vgl_ellipsoid_3d_h_
#define vgl_ellipsoid_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Ellipsoid in 3D, defined by centre point, rotation, 3 axis halflengths
//
// Consider this ellipsoid as being constructed from the unit sphere by first
// extruding along the X, Y and Z axes by the extrusion factors x_halflength(),
// y_halflength() and z_halflength(), then rotating around the origin by the
// given vgl_rotation_3d, and finally translating such that the point (0,0,0)
// moves to the given centre point.
//
// \author Peter Vanroose, Leuven, Belgium
// \date   12 June 2009
//
// \verbatim
//  Modifications
//   Jun 2009 created
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vcl_iosfwd.h>

template <class T>
class vgl_ellipsoid_3d
{
  vgl_point_3d<T> center_;
  T x_halflength_;
  T y_halflength_;
  T z_halflength_;
  vgl_rotation_3d<T> orientation_;

 public:
  //: Default constructor: no initialisations
  vgl_ellipsoid_3d() {}

  //: Construct from a center point, three extrusion factors, and optionally a rotation
  vgl_ellipsoid_3d(vgl_point_3d<T> const& cntr,
                   T x_halflen,
                   T y_halflen,
                   T z_halflen,
                   vgl_rotation_3d<T> const& orient = vgl_rotation_3d<T>())
  : center_(cntr), x_halflength_(x_halflen), y_halflength_(y_halflen), z_halflength_(z_halflen), orientation_(orient) {}

  //: returns the symmetry point (centre) of the ellipsoid
  vgl_point_3d<T> center() const { return center_; }
  //: returns the scaling factor along the first principal axis
  T x_halflength() const { return x_halflength_; }
  //: returns the scaling factor along the second principal axis
  T y_halflength() const { return y_halflength_; }
  //: returns the scaling factor along the third principal axis
  T z_halflength() const { return z_halflength_; }
  //: returns the rotation that created this ellipsoid from an axes-aligned one
  vgl_rotation_3d<T> orientation() const { return orientation_; }

  //: shifts (translates) the ellipsoid by setting its symmetry point (centre)
  void set_center(vgl_point_3d<T> const& cntr) { center_ = cntr; }
  //: sets the rotation that would create this ellipsoid from an axes-aligned one
  void set_orientation(vgl_rotation_3d<T> orient) { orientation_ = orient; }
  //: sets the scalings along the three principal axes
  void set_halflengths(T x, T y, T z) { x_halflength_ = x; y_halflength_ = y; z_halflength_ = z; }

  //: comparison operator
  bool operator==(vgl_ellipsoid_3d<T> const& e) const;

  //: Writes "<vgl_ellipsoid_3d center=(x0,y0,z0), orientation, size=(x_hl,y_hl,z_hl)" to stream
  vcl_ostream& print(vcl_ostream& s) const;
};

//: Writes "<vgl_ellipsoid_3d center=(x0,y0,z0), orientation, size=(x_hl,y_hl,z_hl)" to stream
template <class T>
vcl_ostream& operator<<(vcl_ostream& os, vgl_ellipsoid_3d<T> const& e) { e.print(os); return os; }

#define VGL_ELLIPSOID_3D_INSTANTIATE(T) extern "please include vgl/vgl_ellipsoid_3d.txx first"

#endif // vgl_ellipsoid_3d_h_
