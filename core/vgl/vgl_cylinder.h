// This is core/vgl/vgl_cylinder.h
#ifndef vgl_cylinder_h_
#define vgl_cylinder_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file   vgl_cylinder.h
// \brief  defines a cylinder in 3D by a center point, radius, length and orientation
// \author Gamze D. Tunali (gamze@lems.brown.edu)
// \date   11/07/2005 
//
// \verbatim
//  Modifications
//   \\ Nov 2005 created
// \endverbatim
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

template <class Type>
class vgl_cylinder
{
  vgl_point_3d<Type> center_;
  Type radius_;
  Type length_;
  vgl_vector_3d<Type> orient_;

public:
  //: Default constructor
  vgl_cylinder(void): center_(0.0, 0.0, 0.0), radius_(0.0), length_(0.0){};

  vgl_cylinder(const Type v[8])
    :center_(v[0], v[1], v[2]), radius_(v[3]), length_(v[5]), orient_(vgl_vector_3d<Type> (v[6], v[7], v[8])) {};

  vgl_cylinder(Type cx, Type cy, Type cz, Type radius, Type length)
    :center_(cx, cy, cz), radius_(radius), length_(length), orient_(vgl_vector_3d<Type> (0,0,1)) {};

  vgl_cylinder(vgl_point_3d<Type> center, Type radius, Type length)
    :center_(center), radius_(radius), length_(length), orient_(vgl_vector_3d<Type> (0,0,1)) {};

  vgl_cylinder(vgl_point_3d<Type> center, Type radius, Type length, vgl_vector_3d<Type> orient)
    :center_(center), radius_(radius), length_(length), orient_(orient) {};

  ~vgl_cylinder(void){};

  //: getters
  vgl_point_3d<Type> center() const { return center_; }
  Type radius() const { return radius_; }
  Type length() const { return length_; }
  vgl_vector_3d<Type> orientation() const { return orient_; }

  //: setters
  void set_center(vgl_point_3d<Type> center) { center_ = center; }
  void set_radius(Type radius) { radius_ = radius; }
  void set_length(Type length) { length_ = length; }
  void set_orientation (vgl_vector_3d<Type> orient) { orient_ = orient; }

  //: operations
  bool operator==(vgl_cylinder<Type> const& cyl) const;
  
  //: Writes "<vgl_cylinder center=(x0,y0,z0), radius=r, length=l, direction=(x1,y1,z1)>" to stream
  vcl_ostream& print(vcl_ostream& s) const;
};

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, const vgl_cylinder<T>& cyl);

template <class T>
vcl_istream& operator>>(vcl_istream& s, vgl_cylinder<T>& cyl);

#define VGL_CYLINDER_INSTANTIATE(T) extern "please include vgl/vgl_cylinder.txx first"

#endif
