// This is gel/vifa/vifa_imp_line.h
#ifndef _VIFA_IMP_LINE_H_
#define _VIFA_IMP_LINE_H_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Implicit line segment.
//
// The vifa_imp_line class provides an implicit line segment for computing
// attributes for the edges of intensity faces.  A subclass of the core class
// vgl_line_segment_2d, additional API's for axis projection and parametric
// evaluation are provided.
//
// \author Mike Petersen, June 2003
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_line_segment_2d.h>

template <class Type>
class vifa_imp_line : public vbl_ref_count, public vgl_line_segment_2d<Type>
{
 protected:
  //: X-axis span of line segment
  double dx_;

  //: Y-axis span of line segment
  double dy_;

 public:
  //: Default constructor - does not initialize
  inline vifa_imp_line(void) {}

  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  vifa_imp_line(vifa_imp_line const& l)
    : vbl_ref_count(), vgl_line_segment_2d<Type>(l), dx_(l.dx_), dy_(l.dy_) {}

  //: Line segment constructor
  vifa_imp_line(vgl_point_2d<Type> const& p1,
                vgl_point_2d<Type> const& p2);

  //: Direction/midpoint constructor
  vifa_imp_line(vgl_vector_2d<Type> d,
                vgl_point_2d<Type>  m);

  //: Implicit coefficient constructor
  vifa_imp_line(Type a, Type b, Type c);

  //: Test if a value is near 0.0
  inline bool near_zero(double x) const { return x < 1e-6; }

  //: Compute unit projection along X-axis
  double get_dir_x(void);

  //: Compute unit projection along Y-axis
  double get_dir_y(void);

  //: Compute length of line segment
  double length(void);

  //: Assignment
  void set_points(vgl_point_2d<Type> const& p1,
                  vgl_point_2d<Type> const& p2);

  //: Project a 2D point onto the line
  void project_2d_pt(const Type& p,
                     const Type& q,
                     Type&       x,
                     Type&       y) const;

  vgl_point_2d<Type> project_2d_pt(const vgl_point_2d<Type>& t) const;

  //: Find parametric t-value for a given point relative to line segment.
  double find_t(const vgl_point_2d<Type>& p);

  //: Find point on line (defined by line segment) for a parametric t-value.
  vgl_point_2d<Type> find_at_t(double t);
};

#endif // _VIFA_IMP_LINE_H_
