// This is brl/bseg/sdet/sdet_image_mesh_params.h
#ifndef sdet_image_mesh_params_h_
#define sdet_image_mesh_params_h_
//:
// \file
// \brief parameter mixin for sdet_image_mesh
//
// \author
//    Joseph L. Mundy - March 22, 2011
//    Brown University
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <vbl/vbl_ref_count.h>
#include <gevd/gevd_param_mixin.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>

class sdet_image_mesh_params : public gevd_param_mixin, public vbl_ref_count
{
 public:
  sdet_image_mesh_params(float smooth=1.0f, float thresh=2.0f,
                         int  min_fit_length = 10,
                         double rms_distance = 0.1,
                         double step_half_width = 5.0);

  sdet_image_mesh_params(const sdet_image_mesh_params& old_params);
 ~sdet_image_mesh_params() override = default;

  bool SanityCheck() override;
 friend
  std::ostream& operator<<(std::ostream& os, const sdet_image_mesh_params& imp);
 protected:
  void InitParams(float smooth, float thresh, int min_fit_length,
                  double rms_distance, double step_half_width);
 public:
  //
  // Parameter blocks and parameters
  //
  float smooth_; //!< Gaussian sigma for edge detection
  float thresh_; //!< Edge detection threshold
  int  min_fit_length_; //!< the shortest curve to fit a line
  double rms_distance_; //!< the max rms distance error for the fit
  double step_half_width_; //!< half the step transition width (in pixels)
  std::vector<vgl_point_2d<double> > anchor_points_; //!< additional anchor points to generate 2d mesh

};

#endif // sdet_image_mesh_params_h_
