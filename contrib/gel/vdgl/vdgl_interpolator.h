// This is gel/vdgl/vdgl_interpolator.h
#ifndef vdgl_interpolator_h_
#define vdgl_interpolator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Represents a 2D interpolator for a vdgl_edgel_chain
// \author Geoff Cross
//
// \verbatim
//  J.L. Mundy December 01, 2002
// introduced new method get_tangent_angle, which is based on the actual
// curve geometry.  The old method get_theta strictly refers to the
// stored gradient directions and shouldn't be used for geometric operations
// In this regard, curvature is not a valid computation of geometric curvature
// and should be changed.  Currently it is the rate of change of gradient direction
// \endverbatim
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>

class vdgl_interpolator : public vul_timestamp,
                          public vbl_ref_count
{
   // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors/Destructors--------------------------------------------------

  vdgl_interpolator( vdgl_edgel_chain_sptr chain) : chain_(chain) {};

  // Operators----------------------------------------------------------------

  // interpolation 0th degree
  virtual double get_x(double index)= 0;
  virtual double get_y(double index)= 0;


  // interpolation 1st degree
  virtual double get_grad(double index) = 0;
  virtual double get_theta(double index)= 0;
  virtual double get_tangent_angle(double index)= 0;

  // interpolation 2nd degree
  virtual double get_curvature(double index)= 0;

  // integral
  virtual double get_length()= 0;

  // bounding box
  virtual double get_min_x()= 0;
  virtual double get_max_x()= 0;
  virtual double get_min_y()= 0;
  virtual double get_max_y()= 0;

  // find closest point on the curve to the input point
  // virtual vgl_point_2d<double> closest_point_on_curve2 ( vsol_point_2d_sptr p ) = 0;
  virtual vsol_point_2d_sptr closest_point_on_curve ( vsol_point_2d_sptr p ) = 0;
  virtual double distance_curve_to_point ( vsol_point_2d_sptr p );

  // Data Access---------------------------------------------------------------

  vdgl_edgel_chain_sptr get_edgel_chain() const { return chain_; }

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------

  vdgl_edgel_chain_sptr chain_;
};

#endif // vdgl_interpolator_h_
