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
#include <vbl/vbl_ref_count.h>
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>
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
                                 unsigned k_near,
                                 unsigned block_size,
                                 float weight_offset);
  sdet_texture_classifier_params(const sdet_texture_classifier_params& old_params);
 ~sdet_texture_classifier_params() {}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const sdet_texture_classifier_params& imp);
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
                  unsigned k_near,
                  unsigned block_size,
                  float weight_offset);
 public:
  //
  // Parameter blocks and parameters
  //
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
  //: should the filter response be absolute value vs. signed
  bool mag_;
  //: should the fast algorithm be used to compute the filters
  bool fast_;
  //: the value of k for the k-means algorithm
  unsigned k_;
  //: the number of samples to be randomly selected from the training images
  unsigned n_samples_;
  //: the number of k nearest neighbors
  unsigned k_near_;
  //: the square block size for test images
  unsigned block_size_;
  //: a factor that controls the weighting of textons that appear in more than
  // one class
  float weight_offset_;
};

#endif // sdet_texture_classifier_params_h_
