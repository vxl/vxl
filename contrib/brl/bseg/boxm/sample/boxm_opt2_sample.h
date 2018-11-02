#ifndef boxm_opt2_sample_h_
#define boxm_opt2_sample_h_
//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <boxm/boxm_aux_traits.h>


template <class OBS_T>
class boxm_opt2_sample
{
 public:
  boxm_opt2_sample()
  : obs_(0), pre_(0.0f), vis_(0.0f), PI_(0.0f), seg_len_(0.0f), log_pass_prob_sum_(0.0f), weighted_seg_len_sum_(0.0f)
  {}

  ~boxm_opt2_sample() = default;
  static short version_no() { return 1; }
  void print(std::ostream& os) const;

  OBS_T obs_;
  float pre_;
  float vis_;
  float PI_;
  float seg_len_;

  float log_pass_prob_sum_;
  float weighted_seg_len_sum_;
};

//:traits for a grey optimization sample
template <>
class boxm_aux_traits<BOXM_AUX_OPT2_GREY>
{
 public:
  typedef boxm_opt2_sample<float> sample_datatype;

  static std::string storage_subdir() { return "opt2_grey_work"; }
};

//:traits for an rgb optimization sample
template <>
class boxm_aux_traits<BOXM_AUX_OPT2_RGB>
{
 public:
  typedef boxm_opt2_sample<vil_rgb<float> > sample_datatype;

  static std::string storage_subdir() { return "opt2_rgb_work"; }
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_opt2_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_opt2_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_opt2_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_opt2_sample<T> *&sample);

template <class T>
std::ostream& operator<< (std::ostream& os, const boxm_opt2_sample<T>& sample);

#endif // boxm_opt2_sample_h_
