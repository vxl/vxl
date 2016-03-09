#ifndef boxm_edge_sample_hxx_
#define boxm_edge_sample_hxx_

#include "boxm_edge_sample.h"


// Edge Sample Methods
template <class OBS_T>
void boxm_edge_sample<OBS_T>::print(std::ostream& os) const
{
  os << "(edge_prob=" << edge_prob_ << " num_obs=" << num_obs_ << ')';
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_sample<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.edge_prob_);
  vsl_b_write(os, sample.num_obs_);
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_sample<T> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_sample<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.edge_prob_);
      vsl_b_read(is, sample.num_obs_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_edge_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream& operator << (std::ostream& os, const boxm_edge_sample<T>& sample)
{
  sample.print(os);
  return os;
}

// Auxiliary Edge Sample Methods
template <class OBS_T>
void boxm_aux_edge_sample<OBS_T>::print(std::ostream& os) const
{
  os << "(obs=" << obs_ << " seg_len=" << seg_len_ << ')';
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_aux_edge_sample<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.obs_);
  vsl_b_write(os, sample.seg_len_);
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_aux_edge_sample<T> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_aux_edge_sample<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.obs_);
      vsl_b_read(is, sample.seg_len_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_aux_edge_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_aux_edge_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream& operator << (std::ostream& os, const boxm_aux_edge_sample<T>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_EDGE_SAMPLE_INSTANTIATE(T) \
template class boxm_edge_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_edge_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_edge_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_edge_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_edge_sample<T > *&); \
template std::ostream& operator << (std::ostream&, const boxm_edge_sample<T >&)

#define BOXM_AUX_EDGE_SAMPLE_INSTANTIATE(T) \
template class boxm_aux_edge_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_aux_edge_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_aux_edge_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_aux_edge_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_aux_edge_sample<T > *&); \
template std::ostream& operator << (std::ostream&, const boxm_aux_edge_sample<T >&)

#endif
