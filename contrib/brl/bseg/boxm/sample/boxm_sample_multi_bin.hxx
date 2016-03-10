#ifndef boxm_sample_multi_bin_hxx_
#define boxm_sample_multi_bin_hxx_

#include "boxm_sample_multi_bin.h"
#include <bsta/bsta_mixture_fixed.h>
#include <vsl/vsl_map_io.hxx>
#include <bsta/io/bsta_io_mixture_fixed.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>
#include <bsta/io/bsta_io_mixture.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_gaussian_indep.h>

template <boxm_apm_type APM_MODEL>
typename boxm_sample_multi_bin<APM_MODEL>::apm_datatype & boxm_sample_multi_bin<APM_MODEL>::appearance(int bin)
{
  if (appearance_.find(bin)==appearance_.end())
  {
    appearance_[bin]=apm_datatype();
    return appearance_[bin];
  }
  else
    return appearance_[bin];
}

template <boxm_apm_type APM_MODEL>
void boxm_sample_multi_bin<APM_MODEL>::set_appearance(apm_datatype  app,int bin)
{
  appearance_[bin]=app;
}

template <boxm_apm_type APM_MODEL>
void boxm_sample_multi_bin<APM_MODEL>::print(std::ostream& os) const
{
  os << "(alpha=" << alpha << " appearence=" ;
  for (typename std::map<int,apm_datatype>::const_iterator iter=appearance_.begin();
       iter!=appearance_.end(); ++iter)
    os << "bin#= " << iter->first << ' ' << iter->second;
}

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample_multi_bin<APM_MODEL> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.alpha);
  vsl_b_write(os, sample.appearance_);
}

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample_multi_bin<APM_MODEL> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample_multi_bin<APM_MODEL> &sample)
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample_multi_bin<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample_multi_bin<APM_MODEL> *&sample)
{
  vsl_b_read(is, *sample);
}

template <boxm_apm_type APM_MODEL>
std::ostream& operator << (std::ostream& os, const boxm_sample_multi_bin<APM_MODEL>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_SAMPLE_MULTI_BIN_INSTANTIATE(T) \
template class boxm_sample_multi_bin<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_sample_multi_bin<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_sample_multi_bin<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_sample_multi_bin<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_sample_multi_bin<T > *&); \
template std::ostream& operator << (std::ostream&, const boxm_sample_multi_bin<T >&)

#endif
