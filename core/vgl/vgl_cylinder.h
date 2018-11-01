// This is core/vgl/vgl_cylinder.h
#ifndef vgl_cylinder_h_
#define vgl_cylinder_h_
//:
// \file
// \brief  defines a cylinder in 3D by a center point, radius, length and orientation
// \author Gamze D. Tunali (gamze@lems.brown.edu)
// \date   Nov 07, 2005
//
// \verbatim
//  Modifications
//   Nov 2005 created
// \endverbatim
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class Type>
class vgl_cylinder
{
  vgl_point_3d<Type> center_;
  Type radius_;
  Type length_;
  vgl_vector_3d<Type> orient_;

 public:
  //: Default constructor
  vgl_cylinder(void): center_(0.0, 0.0, 0.0), radius_(0.0), length_(0.0) {}

  vgl_cylinder(const Type v[8])
    :center_(v[0], v[1], v[2]), radius_(v[3]), length_(v[5]), orient_(vgl_vector_3d<Type> (v[6], v[7], v[8])) {}

  vgl_cylinder(Type cx, Type cy, Type cz, Type rad, Type len)
    :center_(cx, cy, cz), radius_(rad), length_(len), orient_(vgl_vector_3d<Type> (0,0,1)) {}

  vgl_cylinder(vgl_point_3d<Type> cntr, Type rad, Type len)
    :center_(cntr), radius_(rad), length_(len), orient_(vgl_vector_3d<Type> (0,0,1)) {}

  vgl_cylinder(vgl_point_3d<Type> cntr, Type rad, Type len, vgl_vector_3d<Type> orient)
    :center_(cntr), radius_(rad), length_(len), orient_(orient) {}

  ~vgl_cylinder(void) = default;

  //: getters
  vgl_point_3d<Type> center() const { return center_; }
  Type radius() const { return radius_; }
  Type length() const { return length_; }
  vgl_vector_3d<Type> orientation() const { return orient_; }

  //: setters
  void set_center(vgl_point_3d<Type> cntr) { center_ = cntr; }
  void set_radius(Type rad) { radius_ = rad; }
  void set_length(Type len) { length_ = len; }
  void set_orientation (vgl_vector_3d<Type> orient) { orient_ = orient; }

  //: operations
  bool operator==(vgl_cylinder<Type> const& cyl) const;

  //: Writes "<vgl_cylinder center=(x0,y0,z0), radius=r, length=l, direction=(x1,y1,z1)>" to stream
  std::ostream& print(std::ostream& s) const;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const vgl_cylinder<T>& cyl);

template <class T>
std::istream& operator>>(std::istream& s, vgl_cylinder<T>& cyl);

#define VGL_CYLINDER_INSTANTIATE(T) extern "please include vgl/vgl_cylinder.hxx first"

#endif // vgl_cylinder_h_
