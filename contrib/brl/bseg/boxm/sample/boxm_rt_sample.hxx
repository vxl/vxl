#ifndef boxm_rt_sample_hxx_
#define boxm_rt_sample_hxx_

#include "boxm_rt_sample.h"


template <class OBS_T>
void boxm_rt_sample<OBS_T>::print(std::ostream& os) const
{
  os << "(obs=" << obs_ << " pre=" << pre_ << " vis=" << vis_ << " PI=" << PI_ << " seg_len=" << seg_len_ << " Beta=" << Beta_ << ')';
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_rt_sample<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.obs_);
  vsl_b_write(os, sample.pre_);
  vsl_b_write(os, sample.vis_);
  vsl_b_write(os, sample.PI_);
  vsl_b_write(os, sample.seg_len_);
  vsl_b_write(os, sample.updatefactor_);
  vsl_b_write(os, sample.Beta_);
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_rt_sample<T> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_rt_sample<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
   case 1:
    vsl_b_read(is, sample.obs_);
    vsl_b_read(is, sample.pre_);
    vsl_b_read(is, sample.vis_);
    vsl_b_read(is, sample.PI_);
    vsl_b_read(is, sample.seg_len_);
    vsl_b_read(is, sample.updatefactor_);
    vsl_b_read(is, sample.Beta_);
    break;
   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_rt_sample<T>&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_rt_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream &operator << (std::ostream& os, const boxm_rt_sample<T>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_RT_SAMPLE_INSTANTIATE(T) \
template class boxm_rt_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_rt_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_rt_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_rt_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_rt_sample<T > *&); \
template std::ostream& operator << (std::ostream&, const boxm_rt_sample<T >&)

#endif
