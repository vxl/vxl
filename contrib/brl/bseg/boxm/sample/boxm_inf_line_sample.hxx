#ifndef boxm_inf_line_sample_hxx_
#define boxm_inf_line_sample_hxx_

#include "boxm_inf_line_sample.h"
#include <vgl/io/vgl_io_infinite_line_3d.h> // for vsl_b_write(os, sample.line_);
#include <vgl/io/vgl_io_line_3d_2_points.h> // for vsl_b_write(os, sample.line_clipped_);

template <class T>
bool boxm_inf_line_sample<T>::operator==(const boxm_inf_line_sample<T> &rhs) const
{
  if ((this==&rhs) || (line_==rhs.line_ && line_clipped_==rhs.line_clipped_ && num_obs_==rhs.num_obs_))
    return true;
  else
    return false;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.num_obs_);
  vsl_b_write(os, sample.residual_);
  vsl_b_write(os, sample.line_);
  vsl_b_write(os, sample.line_clipped_);
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.num_obs_);
      vsl_b_read(is, sample.residual_);
      vsl_b_read(is, sample.line_);
      vsl_b_read(is, sample.line_clipped_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_inf_line_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream& operator << (std::ostream& os, boxm_inf_line_sample<T>& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_INF_LINE_SAMPLE_INSTANTIATE(T) \
template class boxm_inf_line_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_inf_line_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_inf_line_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_inf_line_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_inf_line_sample<T > *&);\
template std::ostream& operator << (std::ostream&, boxm_inf_line_sample<T >&)

#endif
