// This is brl/bseg/sdet/sdet_texture_classifier_params.h
#ifndef sdet_texture_classifier_params_h_
#define sdet_texture_classifier_params_h_
//:
// \file
// \brief parameter mixin for sdet_texture_classifier
//
// \author
//    Joseph L. Mundy - December 10, 2011
//    Brown University
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <vector>
#include <gevd/gevd_param_mixin.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

class sdet_texture_classifier_params : public gevd_param_mixin
{
 public:
  sdet_texture_classifier_params();
  sdet_texture_classifier_params(unsigned n_scales,
                                 float scale_interval,
                                 float angle_interval,
                                 float lambda0,
                                 float lambda1,
                                 float laplace_radius,
                                 float gauss_radius,
                                 float cutoff_per,
                                 bool signed_response,
                                 bool mag,
                                 bool fast,
                                 unsigned k,
                                 unsigned n_samples,
                                 unsigned block_size,
                                 float weight_offset);
  sdet_texture_classifier_params(const sdet_texture_classifier_params& old_params);
 ~sdet_texture_classifier_params() override = default;

  bool SanityCheck() override;
  std::string filter_dir_name();
 friend
  std::ostream& operator<<(std::ostream& os, const sdet_texture_classifier_params& imp);
 protected:
  void InitParams(unsigned n_scales,
                  float scale_interval,
                  float angle_interval,
                  float lambda0,
                  float lambda1,
                  float laplace_radius,
                  float gauss_radius,
                  float cutoff_per,
                  bool signed_response,
                  bool mag,
                  bool fast,
                  unsigned k,
                  unsigned n_samples,
                  unsigned block_size,
                  float weight_offset);
 public:
  // === Parameter blocks and parameters ===

  //: the number of scales used for the anisotropic filters
  unsigned n_scales_;
  //: the scale range for the anisotropic filters
  float scale_interval_;
  //: the angle interval for anisotropic filters
  float angle_interval_;
  //: the major radius of the anisotropic Gaussian filter
  float lambda0_;
  //: the minor radius of the anisotropic Gaussian filter
  float lambda1_;
  //: the radius of the isotropic Laplace filter
  float laplace_radius_;
  //: the radius of the isotropic Gaussian intensity spot filter
  float gauss_radius_;
  //: the residual value of the Gaussian tail considered negligible
  float cutoff_per_;
  //: should the filter response be signed vs. absolute value
  bool signed_response_;
  //: should the filter response be absolute value vs. signed or clipped
  bool mag_;
  //: should the fast algorithm be used to compute the filters
  bool fast_;
  //: the value of k for the k-means algorithm
  unsigned k_;
  //: the number of samples to be randomly selected from the training images
  unsigned n_samples_;
  //: the square block size for test images
  unsigned block_size_;
  //: a factor that controls the weighting of textons that appear in more than one class
  float weight_offset_;
};

//: Binary save vgl_point_2d to stream.
void vsl_b_write(vsl_b_ostream &os,
                 const sdet_texture_classifier_params & tcp);

//: Binary load vgl_point_2d from stream.
void vsl_b_read(vsl_b_istream &is, sdet_texture_classifier_params & tcp);

//: Print human readable summary of object to a stream
void vsl_print_summary(std::ostream& os,
                       const sdet_texture_classifier_params & tcp);

#endif // sdet_texture_classifier_params_h_
