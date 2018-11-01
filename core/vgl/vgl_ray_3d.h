// This is core/vgl/vgl_ray_3d.h
#ifndef vgl_ray_3d_h_
#define vgl_ray_3d_h_
//:
// \file
// \brief A 3-d ray defined by an origin and a direction vector
// \author  J.L. Mundy
//
// \verbatim
// Modifications
// Initial version Sept. 17,  2010
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

//: Represents a 3-d ray
//  The ray origin is p0 and the direction is t_.

template <class Type>
class vgl_ray_3d
{
  vgl_point_3d<Type> p0_; //!< The ray origin
  vgl_vector_3d<Type> t_;  //!< ray direction vector
 public:
  //: Default constructor - does not initialise!
  inline vgl_ray_3d() = default;

  //: Copy constructor
  inline vgl_ray_3d(vgl_ray_3d<Type> const& l)
    : p0_(l.origin()), t_(l.direction()) {}

  //: Construct from orign and direction
  inline vgl_ray_3d(vgl_point_3d<Type> const& p0,
                    vgl_vector_3d<Type> const& direction)
    : p0_(p0), t_(direction) {t_ = t_/static_cast<Type>(t_.length());}

  //: Construct from two points
  inline vgl_ray_3d(vgl_point_3d<Type> const& origin,
                    vgl_point_3d<Type> const& p)
    : p0_(origin), t_(p-origin) {t_ = t_/static_cast<Type>(t_.length());}
  //: Construct from a line segment, direction from 1 to 2
  inline vgl_ray_3d(vgl_line_segment_3d<Type> const& ls)
  {
    p0_ = ls.point1(); t_ = ls.point2()-p0_;
    t_ = t_/static_cast<Type>(t_.length());
  }

  //: Construct from a line 2 points direction from 1 to 2
  inline vgl_ray_3d(vgl_line_3d_2_points<Type> const& ls)
  {
    p0_ = ls.point1(); t_ = ls.point2()-p0_;
    t_ = t_/static_cast<Type>(t_.length());
  }

  //: Destructor
  inline ~vgl_ray_3d() = default;

  //: Accessors
  inline vgl_point_3d<Type> origin() const { return p0_; } // return a copy

  inline vgl_vector_3d<Type> direction() const
  { return t_/static_cast<Type>(t_.length()); } // return a copy

  //: The comparison operator
  inline bool operator==(vgl_ray_3d<Type> const& r) const
  { return (this==&r)||(direction()==r.direction() && origin()==r.origin());}

  inline bool operator!=(vgl_ray_3d<Type>const& other) const
  { return !operator==(other); }

  //: Assignment
  inline void set(vgl_point_3d<Type> const& p0, vgl_vector_3d<Type> const& direction)
  {
    p0_ = p0; t_ = direction;
    t_=t_/static_cast<Type>(t_.length());
  }

  //: Check if point \a p is on the ray and lies in the positive ray direction
  bool contains(const vgl_point_3d<Type>& p ) const;

};

//: Write to stream
// \relatesalso vgl_ray_3d
template <class Type>
std::ostream&  operator<<(std::ostream& s, const vgl_ray_3d<Type>& p);

//: Read from stream
// \relatesalso vgl_ray_3d
template <class Type>
std::istream&  operator>>(std::istream& is,  vgl_ray_3d<Type>& p);
//: public functions
template <class Type>
//: angle between rays
double angle(vgl_ray_3d<Type> const& r0, vgl_ray_3d<Type> const& r1)
{
  return angle(r0.direction(), r1.direction());
}
#define VGL_RAY_3D_INSTANTIATE(T) extern "please include vgl/vgl_ray_3d.hxx first"

#endif // vgl_ray_3d_h_
