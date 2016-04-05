#ifndef boxm_opt3_sample_hxx_
#define boxm_opt3_sample_hxx_

#include "boxm_opt3_sample.h"

#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_io.h>
#include <bsta/bsta_attributes.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_mixture_fixed.h> // for binary io functions
#include <bsta/io/bsta_io_gaussian_indep.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
//#include <boxm/boxm_mog_rgb_processor.h>

template <boxm_apm_type APM>
void boxm_opt3_sample<APM>::print(std::ostream& os) const
{
  os << "(log_pass_prob_sum=" << log_pass_prob_sum_ << " seg_len_sum_=" << seg_len_sum_
     << "weighted_vis_sum_=" << weighted_vis_sum_ << " obs_dist=" << obs_dist_ << ')';
}

template <boxm_apm_type APM>
void boxm_opt3_sample<APM>::update_obs_dist(typename boxm_opt3_sample<APM>::obs_datatype obs, float obs_prob)
{
  boxm_apm_traits<APM>::apm_processor::update(obs_dist_, obs, obs_prob);
}

template <boxm_apm_type APM>
void vsl_b_write(vsl_b_ostream & os, boxm_opt3_sample<APM> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.log_pass_prob_sum_);
  vsl_b_write(os, sample.seg_len_sum_);
  vsl_b_write(os, sample.weighted_vis_sum_);
  vsl_b_write(os, sample.obs_dist_);
}

template <boxm_apm_type APM>
void vsl_b_write(vsl_b_ostream & os, boxm_opt3_sample<APM> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <boxm_apm_type APM>
void vsl_b_read(vsl_b_istream & is, boxm_opt3_sample<APM> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
   case 1:
    vsl_b_read(is, sample.log_pass_prob_sum_);
    vsl_b_read(is, sample.seg_len_sum_);
    vsl_b_read(is, sample.weighted_vis_sum_);
    vsl_b_read(is, sample.obs_dist_);
    break;
   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_opt3_sample<APM>&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

template <boxm_apm_type APM>
void vsl_b_read(vsl_b_istream & is, boxm_opt3_sample<APM> *&sample)
{
  vsl_b_read(is, *sample);
}

template <boxm_apm_type APM>
std::ostream& operator << (std::ostream& os, const boxm_opt3_sample<APM>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_OPT3_SAMPLE_INSTANTIATE(T) \
template class boxm_opt3_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_opt3_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_opt3_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_opt3_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_opt3_sample<T > *&); \
template std::ostream& operator << (std::ostream&, const boxm_opt3_sample<T >&)

#endif
