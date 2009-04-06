#ifndef boxm_sample_txx_
#define boxm_sample_txx_

#include "boxm_sample.h"

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample<APM_MODEL> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.alpha);
  vsl_b_write(os, sample.appearance);
}

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample<APM_MODEL> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample<APM_MODEL> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(os, sample.alpha);
      vsl_b_read(os, sample.appearance);
      break;
}

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample<APM_MODEL> *&sample)
{
  vsl_b_read(is, *sample);
}

#define BOXM_SAMPLE_INSTANTIATE(T) \
template boxm_sample<T>; \
template void vsl_b_write(vsl_b_ostream & os, boxm_sample<T> const &sample); \
template void vsl_b_write(vsl_b_ostream & os, boxm_sample<T> const * &sample); \
template void vsl_b_read(vsl_b_istream & is, boxm_sample<T> &sample); \
template void vsl_b_read(vsl_b_istream & is, boxm_sample<T> *&sample); 

#endif