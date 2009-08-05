#ifndef boxm_appearance_model_traits_h_
#define boxm_appearance_model_traits_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Daniel Crispell
// \date April 03, 2009

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_ostream.h>

class boxm_mog_grey_processor;
class boxm_mog_rgb_processor;
class boxm_simple_grey_processor;

enum boxm_apm_type
{
  BOXM_APM_MOG_GREY = 0,
  BOXM_APM_MOG_RGB,
  BOXM_APM_SIMPLE_GREY,
  BOXM_APM_SIMPLE_RGB,
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
  boxm_simple_grey() : color_(0.5f), one_over_sigma_(1.0f), gauss_weight_(0.0f) {}
  boxm_simple_grey(float color, float std_dev, float gauss_weight) : color_(color), one_over_sigma_(1/std_dev), gauss_weight_(gauss_weight) { check_vals(); }
  boxm_simple_grey(vnl_vector_fixed<float,3> const& params) : color_(params[0]), one_over_sigma_(1.0f/params[1]), gauss_weight_(params[2]) { check_vals(); }
  short version_no() const { return 1; }
  inline float color() const {return color_;}
  inline float sigma() const {return 1.0f/one_over_sigma_;}
  inline float gauss_weight() const {return gauss_weight_;}
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
    if (!(gauss_weight_ > 0.0f))
      gauss_weight_ = 0.0f;
    if (!(gauss_weight_ < 1.0f))
      gauss_weight_ = 1.0f;
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
  static const unsigned int obs_dim = 1;
  static const unsigned int n_params = 3;
  typedef boxm_simple_grey apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
  typedef boxm_simple_grey_processor apm_processor;
};

#if 0
//: traits for a simple one-valued appearance model of rgb images
class boxm_simple_rgb
{
 public:
  boxm_simple_rgb() : color_(0.5f), one_over_sigma_(0.1f) {}
  boxm_simple_rgb(vil_rgb<float> color, vnl_vector_fixed<float,3> std_dev) : color_(color.r, color.g, color.b), one_over_sigma_(1.0f/std_dev[0], 1.0f/std_dev[1], 1.0f/std_dev[2]) { check_vals(); }
  boxm_simple_rgb(vnl_vector_fixed<float,4> &params) : color_(params.extract(3,0)), one_over_sigma_(1.0f/params[3], 1.0f/params[4], 1.0f/params[5]) {check_vals();}


  inline vil_rgb<float> color() const {return vil_rgb<float>(color_[0], color_[1], color_[2]);}
  inline vnl_vector_fixed<float,3> color_vec() const {return color_;}
  inline vnl_vector_fixed<float,3> sigma() const {return vnl_vector_fixed<float,3>(1.0f/one_over_sigma_[0], 1.0f/one_over_sigma_[1], 1.0f/one_over_sigma_[2]);}
  inline vnl_vector_fixed<float,3> one_over_sigma() const {return one_over_sigma_;}

 protected:
  inline void check_vals()
  {
    for (unsigned int i=0; i<3; ++i) {
      if (!(color_[i] > 0.0f))
        color_[i] = 0.0f;
      if (!(color_[i] < 1.0f))
        color_[i] = 1.0f;
      if (!(one_over_sigma_[i] < 1e3))
        one_over_sigma_[i] = 1e3f;
      if (!(one_over_sigma_[i] > 0.1))
        one_over_sigma_[i] = 0.1f;
    }
  }

  vnl_vector_fixed<float,3> color_;
  vnl_vector_fixed<float,3> one_over_sigma_;
};

template<>
class boxm_apm_traits<BOXM_APM_SIMPLE_RGB>
{
 public:
  static const unsigned int obs_dim = 3;
  static const unsigned int n_params = 4;
  typedef boxm_simple_rgb apm_datatype;
  typedef vil_rgb<float> obs_datatype;
  typedef float obs_mathtype;
  typedef boxm_simple_rgb_processor apm_processor;
};

#endif // 0

void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const &sample);
void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const * &sample);
void vsl_b_read(vsl_b_istream & is, boxm_simple_grey &sample);
void vsl_b_read(vsl_b_istream & is, boxm_simple_grey *&sample);
vcl_ostream& operator << (vcl_ostream& os, const boxm_simple_grey& sample);

#endif
