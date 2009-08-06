#ifndef boxm_opt_sample_h_
#define boxm_opt_sample_h_
//:
// \file

#include <vil/vil_rgb.h>
#include <boxm/opt/boxm_aux_traits.h>

template<class OBS_T>
class boxm_opt_sample
{
 public:
  boxm_opt_sample()
    : obs_(0), vis_(0.0f), pre_(0.0f), post_(0.0f), seg_len_(0.0f), PI_(0.0f) {}

  OBS_T obs_;
  float vis_;
  float pre_;
  float post_;
  float seg_len_;
  float PI_;
};

//: traits for a grey optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_GREY>
{
 public:
  typedef boxm_opt_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "opt_grey_work"; }
};

//: traits for an rgb optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RGB>
{
 public:
  typedef boxm_opt_sample<vil_rgb<float> > sample_datatype;

  static vcl_string storage_subdir() { return "opt_rgb_work"; }
};


#endif // boxm_opt_sample_h_
