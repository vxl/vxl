#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include "brec_part_gaussian.h"
//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008

#include "brec_part_gaussian_sptr.h"

#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_threshold.h>
#include <vil/vil_load.h>
#include <brip/brip_vil_float_ops.h>
#include <vnl/vnl_math.h>

#include <bxml/bxml_find.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_gamma.h>

#include <bsta/algo/bsta_fit_weibull.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_gauss_sf1.h>

#include <vul/vul_file.h>
#include <vul/vul_psfile.h>

//: strength_threshold in [0,1] - min strength to declare the part as detected
bool extract_gaussian_primitives(const vil_image_resource_sptr& img, float lambda0, float lambda1, float theta, bool bright, float cutoff_percentage, float strength_threshold, unsigned type, std::vector<brec_part_instance_sptr>& parts)
{
  vil_image_view<float> fimg = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> extr = brip_vil_float_ops::extrema(fimg, lambda0, lambda1, theta, bright, false,true);
  if (extr.nplanes() < 2)
    return false;

  unsigned ni = fimg.ni();
  unsigned nj = fimg.nj();

  vil_image_view<float> res(ni, nj), mask(ni, nj);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      res(i,j) = extr(i,j,0);
      mask(i,j) = extr(i,j,1);
    }

  float min, max;
  vil_math_value_range(res, min, max);

#if 0
  std::cout << "res min: " << min << " max: " << max << std::endl;
  vil_image_view<vxl_byte> res_o(ni, nj);
  vil_convert_stretch_range_limited(res, res_o, min, max);
  vil_save(res_o, "./temp.png");
#endif

  float val = max;
#if 0
  // find the top 10 percentile of the output map and convert it into a prob map (scale to [0,1] range) accordingly
  vil_math_value_range_percentile(res, 1.0, val);
  std::cout << "res top 10 percentile value: " << val << std::endl;
#endif // 0
  vil_image_view<float> strength_map(ni, nj);
  vil_convert_stretch_range_limited(res, strength_map, 0.0f, val, 0.0f, 1.0f);
#if 0
  vil_math_value_range(strength_map, min, max);
  std::cout << "strength_map min: " << min << " max: " << max << std::endl;
  vil_convert_stretch_range_limited(strength_map, res_o, min, max);
  vil_save(res_o, "./strength_map.png");
#endif

  // extract all the parts from the responses
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (strength_map(i,j) > strength_threshold) {
        brec_part_gaussian_sptr dp = new brec_part_gaussian((float)i, (float)j, strength_map(i,j), lambda0, lambda1, theta, bright, type);
        dp->cutoff_percentage_ = cutoff_percentage;
        parts.emplace_back(dp->cast_to_instance());
      }
    }

#if 0
  vil_image_resource_sptr img_resc = vil_new_image_resource_of_view(img);
  vil_image_resource_sptr res_resc = vil_new_image_resource_of_view(res);
  vil_image_resource_sptr msk_resc = vil_new_image_resource_of_view(mask);
  vil_image_view<vil_rgb<vxl_byte> > rgb =
      brip_vil_float_ops::combine_color_planes(img_resc, res_resc, msk_resc);
    vil_save(rgb, "./temp.png");
  vil_math_value_range(fimg, min, max);
  std::cout << "img min: " << min << " max: " << max << std::endl;
  vil_math_value_range(mask, min, max);
  std::cout << "mask min: " << min << " max: " << max << std::endl;

  vil_image_view<bool> res_bool;
  vil_threshold_above(res, res_bool, max/2);
  vil_image_view<float> res_bool_f;
  vil_convert_cast(res_bool, res_bool_f);
  vil_convert_stretch_range_limited(res_bool_f, res_o, 0.0f, 1.0f);
  vil_save(res_o, "./temp_thresholded.png");
#endif

  return true;
}

void brec_part_gaussian::initialize_mask()
{
  vbl_array_2d<float> fa;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, fa, mask_);
  unsigned nrows = fa.rows(), ncols = fa.cols();
  rj_ = (nrows-1)/2;
  ri_ = (ncols-1)/2;
}

brec_part_gaussian::brec_part_gaussian(float x, float y, float strength, float lambda0, float lambda1, float theta, bool bright, unsigned type)
  : brec_part_instance(0, type, brec_part_instance_kind::GAUSSIAN, x, y, strength),
    lambda0_(lambda0), lambda1_(lambda1), theta_(theta), bright_(bright), cutoff_percentage_(0.01f), lambda_(0.0f), k_(0.0f), fitted_weibull_(false)
{
  initialize_mask();
}

//: the following constructor should only be used during parsing
// Mask should be initialized after lambda values are parsed
brec_part_gaussian::brec_part_gaussian() : brec_part_instance(0, 0, brec_part_instance_kind::GAUSSIAN, 0.0f, 0.0f, 0.0f),
    lambda0_(0), lambda1_(0), theta_(0), bright_(true), cutoff_percentage_(0.0f), lambda_(0.0f), k_(0.0f), fitted_weibull_(false)
{
}

brec_part_gaussian* brec_part_gaussian::cast_to_gaussian(void)
{
  return this;
}

bool brec_part_gaussian::mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (img.nplanes() <= plane)
    return false;

  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, kernel, mask);

  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();

  int js = (int)std::floor(y_ - (float)nrows/2.0f + 0.5f);
  int is = (int)std::floor(x_ - (float)ncols/2.0f + 0.5f);
  int je = (int)std::floor(y_ + (float)nrows/2.0f + 0.5f);
  int ie = (int)std::floor(x_ + (float)ncols/2.0f + 0.5f);

  int ni = (int)img.ni();
  int nj = (int)img.nj();
  for (int i = is; i < ie; i++)
    for (int j = js; j < je; j++) {
      int mask_i = i - is;
      int mask_j = j - js;
      if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
        int nw = (int)img(i, j, plane) + int(strength_*255);
        if (nw > 255) img(i, j, plane) = 255;
        else          img(i, j, plane) = (vxl_byte)nw;
      }
    }

  return true;
}

bool brec_part_gaussian::mark_receptive_field(vil_image_view<float>& img, float val)
{
  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, kernel, mask, cutoff_percentage_);

  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();

  int js = (int)std::floor(y_ - (float)nrows/2.0f + 0.5f);
  int is = (int)std::floor(x_ - (float)ncols/2.0f + 0.5f);
  int je = (int)std::floor(y_ + (float)nrows/2.0f + 0.5f);
  int ie = (int)std::floor(x_ + (float)ncols/2.0f + 0.5f);

  int ni = (int)img.ni();
  int nj = (int)img.nj();
  for (int i = is; i < ie; i++)
    for (int j = js; j < je; j++) {
      int mask_i = i - is;
      int mask_j = j - js;
      if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
        /*if ((img(i, j) + val) > 1.0f)
          img(i, j) = 1.0f;
        else
          img(i, j) += val;*/
        if (img(i,j) < val)
          img(i,j) = val;
      }
    }
  return true;
}

bool brec_part_gaussian::mark_center(vil_image_view<float>& img, float val)
{
  int ni = (int)img.ni();
  int nj = (int)img.nj();

  int ic = (int)std::floor(x_ + 0.5f);
  int jc = (int)std::floor(y_ + 0.5f);
  if (ic >= 0 && jc >= 0 && ic < ni && jc < nj)
    if (img(ic, jc) < val)
      img(ic, jc) = val;

  return true;
}

bool brec_part_gaussian::mark_center(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (img.nplanes() <= plane)
    return false;

  int ni = (int)img.ni();
  int nj = (int)img.nj();

  int ic = (int)std::floor(x_ + 0.5f);
  int jc = (int)std::floor(y_ + 0.5f);
  if (ic >= 0 && jc >= 0 && ic < ni && jc < nj)
    img(ic, jc, plane) = (vxl_byte)(strength_*255);

  return true;
}

vnl_vector_fixed<float,2>
brec_part_gaussian::direction_vector(void)  // return a unit vector that gives direction of this instance in the image
{
  vnl_vector_fixed<float,2> v;
  double theta_rad = theta_*vnl_math::pi_over_180;
  v(0) = (float)std::cos(theta_rad);
  v(1) = (float)std::sin(theta_rad);
  return v;
}


bxml_data_sptr brec_part_gaussian::xml_element()
{
  bxml_data_sptr data_super = brec_part_instance::xml_element();

  bxml_element* data = new bxml_element("gaussian");

  data->set_attribute("lambda0",lambda0_);
  data->set_attribute("lambda1",lambda1_);
  data->set_attribute("theta",theta_);
  if (bright_)
    data->set_attribute("bright",1);
  else
    data->set_attribute("bright",0);

  data->set_attribute("cutoff_perc", cutoff_percentage_);
  if (fitted_weibull_)
    data->set_attribute("fitted_weibull", 1);
  else
    data->set_attribute("fitted_weibull", 0);
  data->set_attribute("lambda",lambda_);
  data->set_attribute("k",k_);
  data->set_attribute("lambda_nc",lambda_non_class_);
  data->set_attribute("k_nc",k_non_class_);

  data->append_text("\n ");
  data->append_data(data_super);
  data->append_text("\n ");
#if 0
  ((bxml_element*)data_super.ptr())->append_data(data);
  ((bxml_element*)data_super.ptr())->append_text("\n ");
#endif // 0
  return data;
}

bool brec_part_gaussian::xml_parse_element(bxml_data_sptr data)
{
  bxml_element query("gaussian");
  bxml_data_sptr g_root = bxml_find_by_name(data, query);

  if (!g_root)
    return false;

  if (g_root->type() == bxml_data::ELEMENT) {
    ((bxml_element*)g_root.ptr())->get_attribute("lambda_nc", lambda_non_class_);
    ((bxml_element*)g_root.ptr())->get_attribute("k_nc", k_non_class_);

    bool found = (((bxml_element*)g_root.ptr())->get_attribute("lambda0", lambda0_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("lambda1", lambda1_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("theta", theta_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("cutoff_perc", cutoff_percentage_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("lambda", lambda_) &&
                  ((bxml_element*)g_root.ptr())->get_attribute("k", k_)
                  );

    int bright_int;
    found = found && ((bxml_element*)g_root.ptr())->get_attribute("bright", bright_int);

    if (!found)
      return false;

    bright_ = bright_int != 0;

    int fitted_w_int;
    ((bxml_element*)g_root.ptr())->get_attribute("fitted_weibull", fitted_w_int);
    fitted_weibull_ = fitted_w_int != 0;

    initialize_mask();

    return brec_part_instance::xml_parse_element(g_root);
  }
  else
    return false;
}

std::string brec_part_gaussian::string_identifier()
{
  std::stringstream l0, l1, theta; l0 << lambda0_; l1 << lambda1_; theta << theta_;
  std::string str = "gaussian_"+l0.str()+"_"+l1.str()+"_"+theta.str()+"_";
  if (bright_)
    str = str+"bright";
  else
    str = str+"dark";

  return str;
}

//: the mean img and the std_dev img are float images with values in [0,1] range
bool brec_part_gaussian::construct_bg_response_model(vil_image_view<float>& mean_img,
                                                     vil_image_view<float>& std_dev_img,
                                                     vil_image_view<float> &lambda_img, // output lambda img
                                                     vil_image_view<float> &k_img) // output k img
{
  unsigned ni = mean_img.ni();
  unsigned nj = mean_img.nj();

  // find the response img for this operator
  vil_image_view<float> mean_res = brip_vil_float_ops::extrema(mean_img, lambda0_, lambda1_, theta_, bright_, false, true); // was "false"
#if 0
  vil_save(mean_res, "mean_response_img.tiff");
#endif // 0

  if (mean_res.ni() != ni || mean_res.nj() != nj)
    return false;

  if (k_img.ni()  != ni || k_img.nj() != nj || lambda_img.ni() != ni || lambda_img.nj() != nj)
    return false;

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      double m = mean_res(i,j,1);
      double s = std_dev_img(i,j);
      // very weak response
      if (m<1e-6||s<1e-7) {
        k_img(i,j)=0.0f;
        lambda_img(i,j)= 0.0;
        continue;
      }
      // approximate k
      double k = 1 + 1.21*(m/s-1);
      if (k<1)
        k=1;
      double lambda = m/vnl_gamma((k+1)/k);
      k_img(i,j)= static_cast<float>(k);
      lambda_img(i,j)= static_cast<float>(lambda);
    }

  return true;
}

bool brec_part_gaussian::construct_bg_response_model_gauss(vil_image_view<float>& mean_img, vil_image_view<float>& std_dev_img, vil_image_view<float> &mu_img, vil_image_view<float> &sigma_img)
{
  unsigned ni = mean_img.ni();
  unsigned nj = mean_img.nj();

  // find the response img for this operator
  vil_image_view<float> mean_res = brip_vil_float_ops::extrema(mean_img, lambda0_, lambda1_, theta_, bright_, false, true, true);
  // find the sd dev of the operator at every pixel
  vbl_array_2d<float> kernel; vbl_array_2d<bool> mask;
  brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta_, kernel, mask);
  vil_image_view<float> std_dev_res = brip_vil_float_ops::std_dev_operator_method2(std_dev_img, kernel);

  if (mean_res.ni() != ni || mean_res.nj() != nj)
    return false;

  if (mu_img.ni()  != ni || mu_img.nj() != nj || sigma_img.ni() != ni || sigma_img.nj() != nj)
    return false;

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      float m = mean_res(i,j,2);
      if (bright_)
        m = -m;
      mu_img(i,j)= m;

      float s = std_dev_res(i,j); // was: std_dev_img(i,j);
      sigma_img(i,j)= s;
    }

  return true;
}

//: collect operator responses from the input image
//  Use responses from class regions to estimate lambda and k for the class response model
//  Use responses from non-class regions to estimate lambda and k for the non-class response model
//  Class and non-class regions are specified by the class_prob_img which is a float image with values in [0,1] range
bool brec_part_gaussian::construct_class_response_models(vil_image_view<float>& img,
                                                         vil_image_view<float>& class_prob_img,
                                                         vil_image_view<bool>& mask_img,
                                                         double &lambda, double &k, double &lambda_non_class, double &k_non_class)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();

  // find the response img for this operator
  vil_image_view<float> res = brip_vil_float_ops::extrema(img, lambda0_, lambda1_, theta_, bright_, false, true);
  if (res.ni() != ni || res.nj() != nj)
    return false;

  bsta_histogram<float> h(-7.0f, 1.0f, 32);
  double x_sum = 0.0, xsq_sum = 0.0, p_sum = 0.0;
  double x_sum_non_class = 0.0, xsq_sum_non_class = 0.0, p_sum_non_class = 0.0;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (mask_img(i,j)) {
        float op_res  = res(i,j,0);
        if (op_res < 1.0e-3f)
          continue;

        float prob_class = fg_prob_operator(class_prob_img, i,j);
        float prob_non_class = 1.0f - prob_class;

        x_sum += prob_class*op_res;
        xsq_sum += prob_class*op_res*op_res;
        p_sum += prob_class;

        x_sum_non_class += prob_non_class*op_res;
        xsq_sum_non_class += prob_non_class*op_res*op_res;
        p_sum_non_class += prob_non_class;

        if (prob_class>0.9)
          h.upcount(std::log10(op_res), 1.0f);
      }
    }

  // calculate class parameters
  double mean = x_sum/p_sum; // estimate of mean
  double total_var = xsq_sum/p_sum; // estimate of total variance
  double var = total_var - mean*mean;
  double std_dev = std::sqrt(var);
  std::cout << "Class mean = " << mean << "  std_dev = " << std_dev << '\n';
  bsta_weibull_cost_function wcf(mean, std_dev);
  bsta_fit_weibull<double> fw(&wcf);
  k = 1.001;
  fw.init(k);
  fw.solve(k);

  std::cout << "Class Weibull k fit with residual " << fw.residual() << '\n';
  lambda = fw.lambda(k);
  std::cout << "Class k = " << k << "  lambda = " << lambda << '\n';

  // calculate non-class parameters
  mean = x_sum_non_class/p_sum_non_class; // estimate of mean
  total_var = xsq_sum_non_class/p_sum_non_class; // estimate of total variance
  var = total_var - mean*mean;
  std_dev = std::sqrt(var);
  std::cout << "Non-class mean = " << mean << "  std_dev = " << std_dev << '\n';
  bsta_weibull_cost_function wcfn(mean, std_dev);
  bsta_fit_weibull<double> fwn(&wcfn);
  k_non_class = 1.001;
  fwn.init(k_non_class);
  fwn.solve(k_non_class);

  std::cout << "Class Weibull k fit with residual " << fwn.residual() << '\n';
  lambda_non_class = fwn.lambda(k_non_class);
  std::cout << "Class k = " << k_non_class << "  lambda = " << lambda_non_class << '\n';

  return true;
}

//: for gaussian operators we use weibull distribution as the parametric model
bool brec_part_gaussian::fit_distribution_to_response_hist(bsta_histogram<float>& fg_h)
{
  float mean = fg_h.mean(); auto std_dev = (float)std::sqrt(fg_h.variance());
  bsta_weibull_cost_function wcf(mean, std_dev);
  bsta_fit_weibull<float> fw(&wcf);
  k_ = 1.0f;
  if (fw.init(k_)) {
    fw.solve(k_);
    std::cout << "Weibull k fit with residual " << fw.residual() << '\n';
    lambda_ = fw.lambda(k_);
    std::cout << "k = " << k_ << "  lambda = " << lambda_ << '\n';

    fitted_weibull_ = true;
  }
  else {  // weibull cannot be fit!
    fitted_weibull_ = false;
  }
  return fitted_weibull_;
}


//: find P(alpha in foreground): the probability that this operator alpha in foreground
//  P(alpha in foreground) = argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
float brec_part_gaussian::fg_prob_operator(vil_image_view<float>& fg_prob_img, unsigned i, unsigned j)
{
  float max_prob = 0.0f;
  for (int jj=-rj_; jj<=rj_; ++jj) {
    for (int ii=-ri_; ii<=ri_; ++ii) {
      if (!mask_[jj+rj_][ii+ri_])
        continue;
      else if (fg_prob_img(i+ii, j+jj)>max_prob)
        max_prob=fg_prob_img(i+ii, j+jj);
    }
  }
  return max_prob;
}

//: find P(alpha in background): the probability that this operator alpha is in background
//  P(alpha in background) = 1-argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
float brec_part_gaussian::bg_prob_operator(vil_image_view<float>& fg_prob_img, unsigned i, unsigned j)
{
  float fg_prob = fg_prob_operator(fg_prob_img, i, j);
  return 1.0f-fg_prob;
}


//: collect operator responses from the input image's foreground regions
//  The input img and the fg_prob_img (foreground probability image) are float images with values in [0,1] range
//  Assumes histogram is initialized
bool brec_part_gaussian::update_response_hist(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask_img,
                                              bsta_histogram<float>& fg_h)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();

  // find the response img for this operator
  vil_image_view<float> res = brip_vil_float_ops::extrema(img, lambda0_, lambda1_, theta_, bright_, false, false); // was: "true"
  if (res.ni() != ni || res.nj() != nj)
    return false;

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (mask_img(i,j)) {
        float op_res  = res(i,j);
        if (op_res<=0)
          continue;
        float prob_fore = fg_prob_operator(fg_prob_img, i,j);
        if (prob_fore>0.9)
          fg_h.upcount(op_res, 1.0f); // was: std::log10(op_res)
      }
    }

  return true;
}


//: run the operator on the input img rotated by the given angle and save the instances in the input vector
//  Use the response models saved in the model_dir to set the operator response strength which is equivalent to posterior probability of this pixel's being foreground given the operator response
//  i.e. p(x in foreground | operator response) = p(operator response | x in foreground) / [p(operator response | x in foreground)* + p(operator response | x in background)]
//  Return all the instances which have a posterior larger than zero (--> no thresholding, return "all" the responses)
//  fg_prob_image is the probability of being foreground for each pixel
//  pb_zero is the constant required for the background response model (probability of zero response)
bool brec_part_gaussian::extract(vil_image_view<float>& img, vil_image_view<float>& fg_prob_image,
                                 float rot_angle, const std::string& model_dir, std::vector<brec_part_instance_sptr>& instances, float prior_class)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();

  // find the response img for this operator
  vil_image_view<float> op_res = brip_vil_float_ops::extrema(img, lambda0_, lambda1_, theta_+rot_angle, bright_, false, false);
  if (op_res.ni() != ni || op_res.nj() != nj)
    return false;

  std::string str_id = string_identifier();
  std::string name;
#if 1
  name = "./op_response_img" + str_id + ".tiff";
  vil_save(op_res, name.c_str());
  name = "./prob_image" + str_id + ".tiff";
  vil_save(fg_prob_image, name.c_str());
#endif

  name = model_dir+str_id+"_bg_mu_img.tiff";
  if (!vul_file::exists(name)) {
    std::cerr << "In brec_part_gaussian::extract() -- Problem: Cannot find model parameter file: " << name << "\nNote: train the models and save model param directory in the hierarchy\n;\n";
    return false;
  }
  vil_image_view<float> mu_img = vil_load(name.c_str());

  name = model_dir+str_id+"_bg_sigma_img.tiff";
  if (!vul_file::exists(name)) {
    std::cerr << "In brec_part_gaussian::extract() -- Problem: Cannot find model parameter file: " << name << "\nNote: train the models and save model param directory in the hierarchy\n;\n";
    return false;
  }
  vil_image_view<float> sigma_img = vil_load(name.c_str());

  double lambda, k, lambda_non_class, k_non_class;
  if (fitted_weibull_) {
    std::cout << "using fitted weibull parameters lambda: " << lambda_ << " k: " << k_ << " lambda_nc: " << lambda_non_class_ << " k_nc: " << k_non_class_ << " of the part for the foreground response model!\n";
    lambda = lambda_;  lambda_non_class = lambda_non_class_;
    k = k_;  k_non_class = k_non_class_;
  }
  else {
    name = model_dir+str_id+"_fg_params.txt";

    std::cout << "using weibull parameters from the file: " << name << " for the foreground response model!\n";
    // load the model parameters, check if they exist
    if (!vul_file::exists(name)) {
      std::cerr << "In brec_part_gaussian::extract() -- Problem: Cannot find model parameter file: " << name << "\nNote: train the models and save model param directory in the hierarchy\n;\n";
      return false;
    }

    std::ifstream ifs(name.c_str(), std::ios::in);
    ifs >> k; ifs >> lambda;
    ifs >> k_non_class; ifs >> lambda_non_class;
    ifs.close();
  }

  bsta_weibull<float> pd_class((float)lambda, (float)k);
  bsta_weibull<float> pd_non_class((float)lambda_non_class, (float)k_non_class);
  float prior_non_class = 1.0f - prior_class;

  // extract all the parts from the responses
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      float mu_bk = mu_img(i,j);
      float sigma_bk = sigma_img(i,j);

      float res = op_res(i,j);
      // if the operator response is small we can't tell
      if (res<1.0e-3f)
        continue;

      float pos_c_f = 0.0f;
      float pos_nc_f = 0.0f;
      float pos_c_b = 0.0f;
      float pos_nc_b = 0.0f;

      float pf = fg_prob_operator(fg_prob_image, i, j); // was: fg_prob_image(i, j);

      float pdb = 0.0f;
      bsta_gauss_sf1 pdbg(mu_bk, sigma_bk*sigma_bk);
      pdb = pdbg.prob_density(res);

      float pd_c = pd_class.prob_density(res); // prob density class
      float pd_nc = pd_non_class.prob_density(res); // prob density non_class

      pos_c_f = pd_c*pf*prior_class;
      pos_nc_f = pd_nc*pf*prior_non_class;

      pos_c_b = pdb*(1-pf)*prior_class;
      pos_nc_b = pdb*(1-pf)*prior_non_class;

      float den = pos_c_f + pos_nc_f + pos_c_b + pos_nc_b;
      if (den <= 0.0f)
        continue;  // not a valid part, no detections

      pos_c_f /= den; pos_nc_f /= den; pos_c_b /= den; pos_nc_b /= den;

#ifdef DEBUG
      std::cout << "i: " << i << " j: " << j << " response: " << res
               << " pf: " << pf << " pdf: " << pdf << " pdb: " << pdb
               << " neu: " << neu << " den: " << den << " post: " << posterior << std::endl;
#endif // DEBUG
      brec_part_gaussian_sptr dp = new brec_part_gaussian((float)i, (float)j, res, lambda0_, lambda1_, theta_+rot_angle, bright_, type_);
      dp->rho_c_f_ = pos_c_f;
      dp->rho_nc_f_ = pos_nc_f;
      dp->rho_c_b_ = pos_c_b;
      dp->rho_nc_b_ = pos_nc_b;
      dp->cutoff_percentage_ = cutoff_percentage_;
      instances.emplace_back(dp->cast_to_instance());

#if 0
      if ((i == 375 && j == 204) || (i == 348 && j == 221) || (i == 374 && j == 193) ||
          (i == 229 && j == 366) || (i == 223 && j == 358) || (i == 230 && j == 368) || (i == 227 && j == 364) ||
          (i == 239 && j == 370) || (i == 240 && j == 373) || (i == 235 && j == 369)
         ) {
        std::cout << " i == " << i << " && j == " << j << " \nk_fore: " << k_fore << " lam_fore: " << lambda_fore << " mu_bk: " << mu_bk << " sigma_bk: " << sigma_bk << '\n'
                 << " res: " << res << " pb: " << pb << " pdb: " << pdb << " pdf: " << pdf << " den: " << den << " neu: " << neu << " posterior: " << posterior << std::endl;
      }
#endif
    }

  return true;
}

//: extract and set rho to class probability density of the response
//  Assumes weibull parameters have already been fitted (i.e. fitted_weibull_ = true)
//  This method is to be used during training and it returns an instance if class_prob >= 0.9
bool brec_part_gaussian::extract(vil_image_view<float>& img, vil_image_view<float>& class_prob_image, float rot_angle, std::vector<brec_part_instance_sptr>& instances)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();

  // find the response img for this operator
  vil_image_view<float> op_res = brip_vil_float_ops::extrema(img, lambda0_, lambda1_, theta_+rot_angle, bright_, false, false);
  if (op_res.ni() != ni || op_res.nj() != nj)
    return false;

  if (!fitted_weibull_) {
    std::cout << "ERROR: Foreground response model's parameters have not been set! Run parameter fitting routine first!\n";
    return false;
  }

  bsta_weibull<float> pd_class(lambda_, k_);  bsta_weibull<float> pd_non_class(lambda_non_class_, k_non_class_);

  // extract all the parts from the responses
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      float res = op_res(i,j);
      // if the operator response is small we can't tell
      if (res<1.0e-3f)
        continue;

      float pf = fg_prob_operator(class_prob_image, i, j);
      if (pf < 0.9f)
        continue;

      float pd_c = pd_class.prob_density(res); // prob density class
      float pd_nc = pd_non_class.prob_density(res); // prob density class

      if (pd_c > 0.0f) {
        brec_part_gaussian_sptr dp = new brec_part_gaussian((float)i, (float)j, res, lambda0_, lambda1_, theta_+rot_angle, bright_, type_);
        dp->rho_c_f_ = pd_c;
        dp->rho_nc_f_ = pd_nc;
        dp->rho_c_b_ = 0.0f;
        dp->rho_nc_b_ = 0.0f;

        dp->cutoff_percentage_ = cutoff_percentage_;
        instances.emplace_back(dp->cast_to_instance());
      }
    }

  return true;
}


//: use the background mean and std_dev imgs to construct response model for background and calculate posterior ratio's expected value
//  Assumes that k_ and lambda_ for the foreground response model has already been set
bool brec_part_gaussian::update_foreground_posterior(vil_image_view<float>& img,
                                                     vil_image_view<float>&  /*fg_prob_img*/,
                                                     vil_image_view<bool>&  /*mask*/, // FIXME - unused
                                                     vil_image_view<float>& mean_img,
                                                     vil_image_view<float>& std_dev_img)
{
#if 0
  std::cout << "before update, instance rho_: " << rho_ << '\n'
           << "before update, instance cnt_: " << cnt_ << '\n';
#endif // 0

  unsigned ni = img.ni();
  unsigned nj = img.nj();

  // find the response img for this operator
  vil_image_view<float> op_res = brip_vil_float_ops::extrema(img, lambda0_, lambda1_, theta_, bright_, false, false);
  if (op_res.ni() != ni || op_res.nj() != nj)
    return false;

  vil_image_view<float> mu_img(mean_img.ni(), mean_img.nj());
  vil_image_view<float> sigma_img(mean_img.ni(), mean_img.nj());
  if (!construct_bg_response_model_gauss(mean_img, std_dev_img, mu_img, sigma_img)) {
    std::cout << "In brec_part_gaussian::update_foreground_posterior() - problems in construction of background response model params!\n";
    return false;
  }

#if 0
  bsta_weibull<float> pdfg(lambda_, k_);
#endif // 0

  // extract all the parts from the responses
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      float mu_bk = mu_img(i,j);
      float sigma_bk = sigma_img(i,j);

      float res = op_res(i,j);
      // if the operator response is small we can't tell
      if (res<1.0e-3f)
        continue;

#if 0
      float prob_fore = fg_prob_operator(fg_prob_img, i,j),
            prob_back = 1.0f-prob_fore;
#endif // 0

      bsta_gauss_sf1 pdbg(mu_bk, sigma_bk*sigma_bk);
      float pdb = pdbg.prob_density(res);

      float pdf = 0.0f; // was: = pdfg.prob_density(res); // prob density foreground
      if (fitted_weibull_) {
        bsta_weibull<float> pdfg(lambda_, k_);
        pdf = pdfg.prob_density(res);
      }
      else {
        return false;
      }

      double den = pdb; // was: = pdb*prob_back;
      double neu = pdf; // was: = pdf*prob_fore;
      double rho = 0.0f;
      if (den > 0.0f)
        rho = std::log10(neu/den); // foreground and background posterior ratio

      if (rho > 0.0f) {
        cnt_ = cnt_ + 1;
        rho_c_f_ = ((cnt_-1)*rho_c_f_ + rho)/cnt_;
#ifdef DEBUG
        std::cout << "after update, instance rho_: " << rho_ << " cnt_: " << cnt_
                 << " rho: " << rho << " neu: " << neu << " den: " << den << '\n';
#endif // DEBUG
      }
    }
#ifdef DEBUG
  std::cout << "after update, instance rho_: " << rho_ << '\n'
           << "after update, instance cnt_: " << cnt_ << '\n';
#endif // DEBUG
  return true;
}

bool draw_gauss_to_ps(vul_psfile& ps, const brec_part_gaussian_sptr& pi, float x, float y, float cr, float cg, float cb)
{
  ps.set_fg_color(cr,cg,cb);
  ps.set_line_width(1.f);
  // convert image coordinate system (origin at the top left corner) to ps coordinate system (origin at the bottom left corner)
  ps.ellipse(x*4, -y*4, pi->lambda0_*4, pi->lambda1_*4, int(-pi->theta_));
  return true;
}
