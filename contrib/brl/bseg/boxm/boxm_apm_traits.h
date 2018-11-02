#ifndef boxm_appearance_model_traits_h_
#define boxm_appearance_model_traits_h_
//:
// \file
// \brief A class for appearance model traits
//
// \author Daniel Crispell
// \date April 03, 2009

#include <iostream>
#include <iosfwd>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_beta.h>
#include <bsta/bsta_beta_f1.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <boxm/sample/boxm_edge_sample.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm_export.h"

class boxm_mog_grey_processor;
class boxm_mog_rgb_processor;
class boxm_simple_grey_processor;
class boxm_mob_grey_processor;

enum boxm_apm_type
{
  BOXM_APM_MOG_GREY = 0,
  BOXM_APM_MOG_RGB,
  BOXM_APM_SIMPLE_GREY,
  BOXM_APM_SIMPLE_RGB,
  BOXM_APM_MOB_GREY,
  BOXM_FLOAT,
  BOXM_INT,
  BOXM_SHORT,
  BOXM_CHAR,
  BOXM_BOOL,
  VNL_DOUBLE_10,
  VNL_FLOAT_10,
  VNL_FLOAT_3,
  BSTA_MOG_F1,
  BSTA_GAUSS_F1,
  BVPL_SAMPLE_FLOAT,
  BVPL_SAMPLE_BSTA_GAUSS_F1,
  BVPL_PCA_BASIS_SAMPLE_10,
  BOXM_EDGE_FLOAT,
  BOXM_EDGE_LINE,
  BOXM_EDGE_TANGENT_LINE,
  BOXM_SCALAR_FLOAT,
  DBREC3D_PART_INSTANCE,
  DBREC3D_PAIR_COMPOSITE_INSTANCE,
  BOXM_APM_NA,
  BOXM_APM_UNKNOWN
};

class boxm_apm_types
{
 public:
  static boxm_EXPORT_DATA const char* app_model_strings[];
  static boxm_apm_type str_to_enum(const char* s);
};

//: Pixel properties for templates.
template <boxm_apm_type>
class boxm_apm_traits;


//: traits for a mixture of gaussian appearance model of gray-scale images
template<>
class boxm_apm_traits<BOXM_APM_MOG_GREY>
{
 public:
  static const unsigned int n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type_sf1;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_sf1, n_gaussian_modes_> > mix_gauss_sf1_type;

 public:
  static const unsigned int obs_dim = 1;
  typedef  mix_gauss_sf1_type apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
  typedef boxm_mog_grey_processor apm_processor;
};


//: traits for a mixture of gaussian appearance model of rgb images
template<>
class boxm_apm_traits<BOXM_APM_MOG_RGB>
{
 public:
  static const unsigned int n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type_if3;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_if3, n_gaussian_modes_> > mix_gauss_if3_type;

 public:
  static const unsigned int obs_dim = 3;
  typedef  mix_gauss_if3_type apm_datatype;
  typedef vil_rgb<float> obs_datatype;
  typedef float obs_mathtype;
  typedef boxm_mog_rgb_processor apm_processor;
};


//: traits for a simple one-valued appearance model of grey-scale images
class boxm_simple_grey
{
 public:
  boxm_simple_grey() : color_(0.5f), one_over_sigma_(1.0f),gauss_weight_(1.0f) {}
  boxm_simple_grey(float colour, float std_dev, float gauss_wght=1.0f) : color_(colour), one_over_sigma_(1/std_dev), gauss_weight_(gauss_wght)  { check_vals(); }
  boxm_simple_grey(vnl_vector_fixed<float,3> const& params) : color_(params[0]), one_over_sigma_(1.0f/params[1])  { check_vals(); }
  static short version_no() { return 1; }
  inline float color() const { return color_; }
  inline float sigma() const { return 1.0f/one_over_sigma_; }
  inline float gauss_weight() const { return gauss_weight_; }
  inline float one_over_sigma() const { return one_over_sigma_; }

 protected:
  inline void check_vals()
  {
    if (!(color_ > 0.0f))
      color_ = 0.0f;
    if (!(color_ < 1.0f))
      color_ = 1.0f;
    if (!(one_over_sigma_ < 1e4))
      one_over_sigma_ = 1e4f;
    if (!(one_over_sigma_ > 1e-4))
      one_over_sigma_ = 1e-4f;
  }

 public:
  float color_;
  float one_over_sigma_;
  float gauss_weight_;
};

template<>
class boxm_apm_traits<BOXM_APM_SIMPLE_GREY>
{
 public:
  static const unsigned int n_gaussian_modes_ = 1;
  static const unsigned int obs_dim = 1;
  static const unsigned int n_params = 3;
  typedef boxm_simple_grey apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
  typedef boxm_simple_grey_processor apm_processor;
};

template<>
class boxm_apm_traits<BOXM_APM_MOB_GREY>
{
 public:
  static const unsigned int n_beta_modes_ = 3;

  typedef bsta_num_obs<bsta_beta_f1> beta_type;
  typedef bsta_num_obs<bsta_mixture_fixed<beta_type, n_beta_modes_> > mix_beta_type;

 public:
  static const unsigned int obs_dim = 1;
  typedef  mix_beta_type apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
  typedef boxm_mob_grey_processor apm_processor;
};

template<>
class boxm_apm_traits<BOXM_EDGE_FLOAT>
{
 public:
  static const unsigned int obs_dim = 1;
  static const unsigned int n_params = 2;
  typedef boxm_edge_sample<float> apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
#if 0
  typedef boxm_edge_processor apm_processor;
#endif
};

template<>
class boxm_apm_traits<BOXM_EDGE_LINE>
{
 public:
  static const unsigned int obs_dim = 1;
  static const unsigned int n_params = 2;
  typedef boxm_inf_line_sample<float> apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
};

void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const &sample);
void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const * &sample);
void vsl_b_read(vsl_b_istream & is, boxm_simple_grey &sample);
void vsl_b_read(vsl_b_istream & is, boxm_simple_grey *&sample);
std::ostream& operator << (std::ostream& os, const boxm_simple_grey& sample);

#endif
