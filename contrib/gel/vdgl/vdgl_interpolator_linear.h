// This is gel/vdgl/vdgl_interpolator_linear.h
#ifndef vdgl_interpolator_linear_h
#define vdgl_interpolator_linear_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Represents a linear 2D interpolator for a vdgl_edgel_chain
// \author Geoff Cross

#include <vdgl/vdgl_interpolator.h>

class vdgl_interpolator_linear : public vdgl_interpolator
{
 public:

  // Constructors/Destructors--------------------------------------------------

  vdgl_interpolator_linear( vdgl_edgel_chain_sptr chain);
  ~vdgl_interpolator_linear();

  // Operators----------------------------------------------------------------

  //: interpolation 0th degree
  double get_x(double index);
  double get_y(double index);

  // interpolation 1st degree
  double get_grad(double index);
  //: the image gradient direction
  double get_theta(double index);
  //:  the geometric tangent angle
  double get_tangent_angle(double index);

  //: interpolation 2nd degree
  double get_curvature(double index);

  //: integral
  double get_length();

  //: bounding box
  double get_min_x();
  double get_max_x();
  double get_min_y();
  double get_max_y();

  // closest point
  vsol_point_2d_sptr closest_point_on_curve ( vsol_point_2d_sptr p );

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------

  double lengthcache_;
  double minxcache_;
  double maxxcache_;
  double minycache_;
  double maxycache_;

 private:
  // Helpers-------------------------------------------------------------------

  void recompute_all();
  void recompute_length();
  void recompute_bbox();
};

#endif // vdgl_interpolator_linear_h
