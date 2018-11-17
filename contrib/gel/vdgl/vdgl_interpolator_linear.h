// This is gel/vdgl/vdgl_interpolator_linear.h
#ifndef vdgl_interpolator_linear_h
#define vdgl_interpolator_linear_h
//:
// \file
// \brief Represents a linear 2D interpolator for a vdgl_edgel_chain
// \author Geoff Cross

#include <vdgl/vdgl_interpolator.h>

class vdgl_interpolator_linear : public vdgl_interpolator
{
 public:
  // Constructors/Destructors--------------------------------------------------

  vdgl_interpolator_linear( const vdgl_edgel_chain_sptr& chain);
  ~vdgl_interpolator_linear() override;

  // Operators----------------------------------------------------------------

  //: order of interpolation 1=linear, 2 = quadratic, 3 = cubic, ..etc.
  short order() const override {return 1;}

  //: interpolation 0th degree
  double get_x(double index) override;
  double get_y(double index) override;

  // interpolation 1st degree
  double get_grad(double index) override;
  //: the image gradient direction
  double get_theta(double index) override;
  //:  the geometric tangent angle
  double get_tangent_angle(double index) override;

  //: interpolation 2nd degree
  double get_curvature(double index) override;

  //: integral
  double get_length() override;

  //: bounding box
  double get_min_x() override;
  double get_max_x() override;
  double get_min_y() override;
  double get_max_y() override;

  // closest point
  vsol_point_2d_sptr closest_point_on_curve ( vsol_point_2d_sptr p ) override;

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
