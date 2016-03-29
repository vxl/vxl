#ifndef boxm_sample_hxx_
#define boxm_sample_hxx_

#include "boxm_sample.h"
#include <bsta/bsta_mixture_fixed.h>

template <boxm_apm_type APM_MODEL>
void boxm_sample<APM_MODEL>::print(std::ostream& os) const
{
  os << "(alpha=" << alpha << " appearence=" << appearance_ << ')';
}

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample<APM_MODEL> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.alpha);
  vsl_b_write(os, sample.appearance_);
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
      vsl_b_read(is, sample.alpha);
      vsl_b_read(is, sample.appearance_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample<APM_MODEL> *&sample)
{
  vsl_b_read(is, *sample);
}

template <boxm_apm_type APM_MODEL>
std::ostream& operator << (std::ostream& os, const boxm_sample<APM_MODEL>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_SAMPLE_INSTANTIATE(T) \
template class boxm_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_sample<T > *&); \
template std::ostream& operator << (std::ostream&, const boxm_sample<T >&)

#endif
