// This is brl/bseg/brec/brec_part_gaussian.h
#ifndef brec_part_gaussian_h_
#define brec_part_gaussian_h_
//:
// \file
// \brief class to represent primitive parts as non-isotropic gaussian filters oriented in various ways
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vector>
#include <iostream>
#include "brec_part_base.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "brec_part_gaussian_sptr.h"

#include <vnl/vnl_vector_fixed.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include <bsta/bsta_weibull.h>

#include <vul/vul_psfile.h>

class brec_part_gaussian : public brec_part_instance
{
 public:

  brec_part_gaussian(float x, float y, float strength, float lambda0, float lambda1, float theta, bool bright, unsigned type);

  //: the following constructor should only be used during parsing
  brec_part_gaussian();

  bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane) override;
  bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane) override;
  bool mark_receptive_field(vil_image_view<float>& img, float val) override;
  bool mark_center(vil_image_view<float>& img, float val) override;

  vnl_vector_fixed<float,2> direction_vector(void) override;  // return a unit vector that gives direction of this instance in the image

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const override
  {
    os << "x: " << x_ << " y: " << y_ << " strength: " << strength_ << std::endl
       << "lambda0: " << lambda0_ << " lambda1: " << lambda1_ << " theta: " << theta_ << std::endl;
  }

  brec_part_gaussian* cast_to_gaussian(void) override;

  bxml_data_sptr xml_element() override;
  bool xml_parse_element(bxml_data_sptr data) override;
  void initialize_mask();

  //: run the operator on \p mean_img and use the response to construct a response model for this operator
  //  The \p mean_img and the \p std_dev_img are float images with values in [0,1] range
  //  They are supposedly extracted from the background model of the scene
  bool construct_bg_response_model(vil_image_view<float>& mean_img, vil_image_view<float>& std_dev_img, vil_image_view<float> &lambda_img, vil_image_view<float> &k_img);
  bool construct_bg_response_model_gauss(vil_image_view<float>& mean_img, vil_image_view<float>& std_dev_img, vil_image_view<float> &mu_img, vil_image_view<float> &sigma_img);

  //: collect operator responses from the input image
  //  Use responses from class regions to estimate lambda and k for the class response model
  //  Use responses from non-class regions to estimate lambda and k for the non-class response model
  //  Class and non-class regions are specified by the class_prob_img which is a float image with values in [0,1] range
  bool construct_class_response_models(vil_image_view<float>& img, vil_image_view<float>& class_prob_img,
    vil_image_view<bool>& mask_img, double &lambda, double &k, double &lambda_non_class, double &k_non_class);

  //: collect operator responses from the input image's foreground regions
  //  The input \p img and the \p fg_prob_img (foreground probability image) are float images with values in [0,1] range
  //  Assumes histogram is initialized
  bool update_response_hist(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask_img, bsta_histogram<float>& fg_h) override;
  //: for gaussian operators we use weibull distribution as the parametric model
  bool fit_distribution_to_response_hist(bsta_histogram<float>& fg_h) override;

  //: use the background \p mean_img and \p std_dev_img to construct response model for background and calculate posterior ratio of foreground and background
  //  Assumes that \p k_ and \p lambda_ for the foreground response model have already been set
  bool update_foreground_posterior(vil_image_view<float>& inp,
                                           vil_image_view<float>& fg_prob_img,
                                           vil_image_view<bool>& mask,
                                           vil_image_view<float>& mean_img,
                                           vil_image_view<float>& std_dev_img) override;

  //: run the operator on the input \p img rotated by the given angle and save the instances in the input vector
  //  Use the response models saved in the \p model_dir to set the operator response strength which is equivalent to posterior probability of this pixel being foreground given the operator response
  //  i.e. p(x in foreground | operator response) = p(operator response | x in foreground) / [p(operator response | x in foreground) + p(operator response | x in background)]
  //  \return all the instances which have a posterior larger than zero (--> no thresholding, return "all" the responses)
  //  \p fg_prob_img is the probability of being foreground for each pixel
  //  \p pb_zero is the constant required for the background response model (probability of zero response)
  bool extract(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, float rot_angle, const std::string& model_dir, std::vector<brec_part_instance_sptr>& instances, float prior_class);

  //: extract and set rho to class probability density of the response
  //  Assumes weibull parameters have already been fitted (i.e. fitted_weibull_ = true)
  //  This method is to be used during training and it returns an instance if class_prob >= 0.9
  bool extract(vil_image_view<float>& img, vil_image_view<float>& class_prob_image, float rot_angle, std::vector<brec_part_instance_sptr>& instances);

  //: find P(alpha in foreground): the probability that this operator alpha is in foreground
  //  P(alpha in foreground) = argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
  float fg_prob_operator(vil_image_view<float>& fg_prob_img, unsigned i, unsigned j);
  //: find P(alpha in background): the probability that this operator alpha is in background
  //  P(alpha in background) = 1-argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
  float bg_prob_operator(vil_image_view<float>& fg_prob_img, unsigned i, unsigned j);

  std::string string_identifier();

 public:
  float lambda0_;  // axis
  float lambda1_;
  float theta_;    // orientation angle (in degrees)
  bool bright_;

  //: parameter to define how big a receptive field will be marked, default is 0.01 so 1% of the tails of the gaussian is cut off
  float cutoff_percentage_;

  vbl_array_2d<bool> mask_;
  int rj_, ri_;

  float lambda_, k_;  // we fit Weibull distribution to Gaussian operators' response model
  float lambda_non_class_, k_non_class_;  // we fit Weibull distribution to Gaussian operators' response model
  bool fitted_weibull_;
};

//: extracts only one type of primitive and adds to the part vector
//  Strength_threshold in [0,1] - min strength to declare the part as detected
bool extract_gaussian_primitives(const vil_image_resource_sptr& img, float lambda0, float lambda1, float theta, bool bright, float cutoff_percentage, float strength_threshold, unsigned type, std::vector<brec_part_instance_sptr>& parts);

bool draw_gauss_to_ps(vul_psfile& ps, const brec_part_gaussian_sptr& pi, float x, float y, float cr, float cg, float cb);

#endif // brec_part_gaussian_h_
