// This is brl/bseg/sdet/sdet_denoise_mrf_bp.h
#ifndef sdet_denoise_mrf_bp_h_
#define sdet_denoise_mrf_bp_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor applying a mrf denoise algorithm
//
// \author
//  J.L. Mundy - March 30, 2011
//
//  This algorithm selectively smooths the image based on
//  a variance value at each pixel. The smoothing is carried out
//  by a MRF with binary cliques all of equal weight (kappa_)
//  The data cost is related to the variance by
//
//  D(fp) = lambda_*(fp-x)^2
//         -------
//           var
//  The clique cost is V(fp, fq) = kappa_(fp-fq)^2
//
//  The algorithm uses belief propagation based on the paper by
//
//  Pedro F. Felzenszwalb, Daniel P. Huttenlocher,
//  Efficient Belief Propagation for Early Vision
//  International Journal of Computer Vision 70(1): 41-54 (2006)
//
//  The MRF message storage could be reduced by 1/2
//  if a checkerboard update scheme is used, but it was decided to
//  update all sites on each iteration.
//
//  If a variance image is not set, then the data cost is
//
//   D(fp) = lambda_*(fp-x)^2
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <sdet/sdet_denoise_mrf_bp_params.h>
#include <sdet/sdet_mrf_bp.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pyramid_image_view.h>

class sdet_denoise_mrf_bp : public sdet_denoise_mrf_bp_params
{
 public:
  //:Constructors/destructor
  sdet_denoise_mrf_bp(sdet_denoise_mrf_bp_params& imp);
  ~sdet_denoise_mrf_bp() override;
  //: Process methods

  void set_image(vil_image_resource_sptr const& resource);
  void set_variance(vil_image_resource_sptr const& var_resc);

  //: the full denoising process
  bool denoise();

  //: Accessors
  vil_image_resource_sptr output() { return out_resc_; }
  bool output_valid() const { return output_valid_; }

  //: Internals (for debug purposes)
  sdet_mrf_bp_sptr mrf() { return mrf_; }

 protected:
  // === protected methods ===

  //: upsample the messages from a mrf by a factor of two.
  // \p level is the pyramid level of the input mrf.
  // the returned mrf is at the next higher resolution level of the pyramid.
  // the prior messages of the returned mrf are initialized be the message
  // values of in_mrf.
  sdet_mrf_bp_sptr pyramid_upsample(sdet_mrf_bp_sptr const& in_mrf,
                                    unsigned level);

  // === members ===

  bool output_valid_;      //!< process state flag
  bool use_var_;
  vil_pyramid_image_view<float> pyr_in_;
  vil_pyramid_image_view<float> pyr_var_;
  vil_image_resource_sptr out_resc_;
  sdet_mrf_bp_sptr mrf_;
};

#endif // sdet_denoise_mrf_bp_h_
