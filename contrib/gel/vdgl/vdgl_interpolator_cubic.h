// This is gel/vdgl/vdgl_interpolator_cubic.h
#ifndef vdgl_interpolator_cubic_h
#define vdgl_interpolator_cubic_h
//:
// \file
// \brief Represents a 2D interpolator_cubic for a vdgl_edgel_chain
// \author
//  Geoff Cross
//
// \verbatim
//  Modifications
//   4-Dec-2002  Peter Vanroose  Implemented get_tangent_angle
// \endverbatim

#include <vdgl/vdgl_interpolator.h>

//: Represents a 2D interpolator_cubic for a vdgl_edgel_chain

class vdgl_interpolator_cubic : public vdgl_interpolator
{
 public:

  // Constructors/Destructors--------------------------------------------------

  vdgl_interpolator_cubic( const vdgl_edgel_chain_sptr& chain);
  ~vdgl_interpolator_cubic() override;

  // Operators----------------------------------------------------------------

  //: order of interpolation 1=linear, 2 = quadratic, 3 = cubic, ..etc.
  short order() const override {return 3;}

  //: interpolation 0th degree
  double get_x(double index) override;
  double get_y(double index) override;

  //: interpolation 1st degree
  double get_grad(double index) override;
  double get_theta(double index) override;
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

#endif // vdgl_interpolator_cubic_h
