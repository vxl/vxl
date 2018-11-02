#ifndef boxm_rt_sample_h_
#define boxm_rt_sample_h_
//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <boxm/boxm_aux_traits.h>

template<class OBS_T>
class boxm_rt_sample
{
 public:
  boxm_rt_sample()
    : obs_(0), pre_(0.0f), vis_(0.0f), PI_(0.0f), seg_len_(0.0f), Beta_(0.0f),
    updatefactor_(0.0f), log_sum_(0.0f) {}
  ~boxm_rt_sample() = default;
  static short version_no() { return 1; }
  void print(std::ostream& os) const;

  OBS_T obs_;
  float pre_;
  float vis_;
  float PI_;
  float seg_len_;
  float Beta_;
  float updatefactor_;
  float log_sum_;
};


//: traits for a grey optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>
{
 public:
  typedef boxm_rt_sample<float> sample_datatype;

  static std::string storage_subdir() { return "opt_rt_grey_work"; }
};

//: traits for an rgb optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_RGB>
{
 public:
  typedef boxm_rt_sample<vil_rgb<float> > sample_datatype;

  static std::string storage_subdir() { return "opt_rt_rgb_work"; }
};
template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_rt_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_rt_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_rt_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_rt_sample<T> *&sample);

template <class T>
std::ostream& operator << (std::ostream& os, const boxm_rt_sample<T>& sample);

#endif // boxm_rt_sample_h_
