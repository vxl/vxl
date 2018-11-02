#ifndef boxm_opt3_sample_h_
#define boxm_opt3_sample_h_
//:
// \file

#include <vector>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>

//: class for holding auxiliary information for opt3 process.
// Note that APM is not necessarily appearance model type of world,
// but observation types of the two models should be the same.
template<boxm_apm_type APM>
class boxm_opt3_sample
{
 public:
  typedef typename boxm_apm_traits<APM>::obs_datatype obs_datatype;
  typedef typename boxm_apm_traits<APM>::apm_datatype apm_datatype;

  boxm_opt3_sample()
    : weighted_vis_sum_(0.0f), seg_len_sum_(0.0f), log_pass_prob_sum_(0.0f) { }
  ~boxm_opt3_sample() = default;
  static short version_no() { return 1; }
  void print(std::ostream& os) const;

  void update_obs_dist(obs_datatype obs, float obs_prob);

  apm_datatype obs_dist_; // distribution of samples from this observation
  float weighted_vis_sum_; // sum of visibility probabilities * corresponding seg_lens
  float seg_len_sum_; // sum of sample segment lengths
  float log_pass_prob_sum_; // sum of log of updated (1 - PQ)
};

//: traits for a grey optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT3_MOG_GREY>
{
 public:
   typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype sample_datatype;
   static const boxm_apm_type APM_TYPE = BOXM_APM_MOG_GREY;

  static std::string storage_subdir() { return "opt3_mog_grey"; }
};

//: traits for an rgb optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT3_MOG_RGB>
{
 public:
   typedef boxm_apm_traits<BOXM_APM_MOG_RGB>::obs_datatype sample_datatype;
   static const boxm_apm_type APM_TYPE = BOXM_APM_MOG_RGB;

  static std::string storage_subdir() { return "opt3_mog_rgb"; }
};

template <boxm_apm_type APM>
void vsl_b_write(vsl_b_ostream & os, boxm_opt3_sample<APM> const &sample);

template <boxm_apm_type APM>
void vsl_b_write(vsl_b_ostream & os, boxm_opt3_sample<APM> const * &sample);

template <boxm_apm_type APM>
void vsl_b_read(vsl_b_istream & is, boxm_opt3_sample<APM> &sample);

template <boxm_apm_type APM>
void vsl_b_read(vsl_b_istream & is, boxm_opt3_sample<APM> *&sample);

template <boxm_apm_type APM>
std::ostream& operator << (std::ostream& os, const boxm_opt3_sample<APM>& sample);

#endif // boxm_opt3_sample_h_
