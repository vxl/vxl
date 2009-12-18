#ifndef boxm_appearance_model_traits_h_
#define boxm_appearance_model_traits_h_
//:
// \file
// \brief A class for appearance model traits
//
// \author Daniel Crispell
// \date April 03, 2009

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_beta.h>
#include <bsta/bsta_beta_f1.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_ostream.h>

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
  FLOAT,
  BSTA_MOG_F1,
  BSTA_F1,
  BVPL_SAMPLE_FLOAT,
  BVPL_SAMPLE_BSTA_GAUSS_F1,
  BOXM_APM_NA,
  BOXM_APM_UNKNOWN  
};

class boxm_apm_types
{
 public:
  static const char* app_model_strings[];
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

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type_f1;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_f1, n_gaussian_modes_> > mix_gauss_f1_type;

 public:
  static const unsigned int obs_dim = 1;
  typedef  mix_gauss_f1_type apm_datatype;
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
  boxm_simple_grey() : color_(0.5f), one_over_sigma_(1.0f) {}
  boxm_simple_grey(float color, float std_dev, float gauss_weight=1.0f) : color_(color), one_over_sigma_(1/std_dev)  { check_vals(); }
  boxm_simple_grey(vnl_vector_fixed<float,3> const& params) : color_(params[0]), one_over_sigma_(1.0f/params[1])  { check_vals(); }
  static short version_no() { return 1; }
  inline float color() const {return color_;}
  inline float sigma() const {return 1.0f/one_over_sigma_;}
  inline float gauss_weight() const {return 1.0f;}
  inline float one_over_sigma() const {return one_over_sigma_;}

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
};

template<>
class boxm_apm_traits<BOXM_APM_SIMPLE_GREY>
{
 public:
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

void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const &sample);
void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const * &sample);
void vsl_b_read(vsl_b_istream & is, boxm_simple_grey &sample);
void vsl_b_read(vsl_b_istream & is, boxm_simple_grey *&sample);
vcl_ostream& operator << (vcl_ostream& os, const boxm_simple_grey& sample);

#endif
