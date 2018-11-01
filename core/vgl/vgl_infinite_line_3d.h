// This is core/vgl/vgl_infinite_line_3d.h
#ifndef vgl_infinite_line_3d_h_
#define vgl_infinite_line_3d_h_
//:
// \file
// \brief A 3-d infinite line with position parameterized by orthogonal plane coordinates
// \author  J.L. Mundy
//
// \verbatim
// Modifications
// Initial version July 25, 2009
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

//: Represents a 3-d line with position defined in the orthogonal plane passing through the origin.
//  The line direction is t_.
//  The 2-d plane coordinate system (u, v) is aligned with the 3-d coordinate
//  system (X, Y, X), where v = t x X and u = v x t.
template <class Type>
class vgl_infinite_line_3d
{
  vgl_vector_2d<Type> x0_; //!< line position vector
  vgl_vector_3d<Type> t_;  //!< line direction vector (tangent)
 public:
  //: Default constructor - does not initialise!
  inline vgl_infinite_line_3d() = default;

  //: Copy constructor
  inline vgl_infinite_line_3d(vgl_infinite_line_3d<Type> const& l)
    : x0_(l.x0()), t_(l.direction()) {}

  //: Construct from x0 and direction
  inline vgl_infinite_line_3d(vgl_vector_2d<Type> const& x_0,
                              vgl_vector_3d<Type> const& direction)
    : x0_(x_0), t_(direction) {}

  //: Construct from two points
  vgl_infinite_line_3d(vgl_point_3d<Type> const& p1,
                       vgl_point_3d<Type> const& p2);

  //: Construct from a point and direction
  vgl_infinite_line_3d(vgl_point_3d<Type> const& p,
                       vgl_vector_3d<Type> const& direction);

  //: Construct from a line segment
  inline vgl_infinite_line_3d(vgl_line_segment_3d<Type> const& ls)
  {
    vgl_infinite_line_3d<Type> inf_l(ls.point1(), ls.point2());
    x0_ = inf_l.x0(); t_ = inf_l.direction();
  }

  //: Construct from a line 2 points
  inline vgl_infinite_line_3d(vgl_line_3d_2_points<Type> const& ls)
  {
    vgl_infinite_line_3d<Type> inf_l(ls.point1(), ls.point2());
    x0_ = inf_l.x0(); t_ = inf_l.direction();
  }

  //: Destructor
  inline ~vgl_infinite_line_3d() = default;

  //: Accessors
  inline vgl_vector_2d<Type> x0() const { return x0_; } // return a copy
  inline vgl_vector_3d<Type> direction() const
  { return t_/static_cast<Type>(t_.length()); } // return a copy

  //: The comparison operator
  inline bool operator==(vgl_infinite_line_3d<Type> const& l) const
  { return (this==&l) || (direction() == l.direction() && x0() == l.x0()); }

  inline bool operator!=(vgl_infinite_line_3d<Type>const& other) const
  { return !operator==(other); }

  //: Assignment
  inline void set(vgl_vector_2d<Type> const& x_0, vgl_vector_3d<Type> const& direction)
  { x0_ = x_0; t_ = direction; }

  //: Return the point on the line closest to the origin
  vgl_point_3d<Type> point() const;

  //: Return a point on the line defined by a scalar parameter \a t.
  // \a t=0.0 corresponds to the closest point on the line to the origin
  vgl_point_3d<Type> point_t(const double t) const { return point() + t*direction(); }

  //: Check if point \a p is on the line
  bool contains(const vgl_point_3d<Type>& p ) const;

  //: The unit vectors perpendicular to the line direction
  void compute_uv_vectors(vgl_vector_3d<Type>& u, vgl_vector_3d<Type>& v) const;
};

//: Write to stream
// \relatesalso vgl_infinite_line_3d
template <class Type>
std::ostream&  operator<<(std::ostream& s, const vgl_infinite_line_3d<Type>& p);

//: Read from stream
// \relatesalso vgl_infinite_line_3d
template <class Type>
std::istream&  operator>>(std::istream& is,  vgl_infinite_line_3d<Type>& p);
#define VGL_INFINITE_LINE_3D_INSTANTIATE(T) extern "please include vgl/vgl_infinite_line_3d.hxx first"

#endif // vgl_infinite_line_3d_h_
