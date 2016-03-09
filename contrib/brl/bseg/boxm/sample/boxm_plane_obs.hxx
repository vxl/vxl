#ifndef boxm_plane_obs_hxx_
#define boxm_plane_obs_hxx_

#include "boxm_plane_obs.h"

#include "boxm_edge_tangent_sample.h"
#include "boxm_inf_line_sample.h"

#include <vgl/io/vgl_io_homg_plane_3d.h> // for vsl_b_write(os, sample.plane_);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_plane_obs<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.seg_len_);
  vsl_b_write(os, sample.plane_);
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_plane_obs<T> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_plane_obs<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.seg_len_);
      vsl_b_read(is, sample.plane_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_edge_tangent_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_plane_obs<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream& operator<< (std::ostream& os, boxm_plane_obs<T> const& sample)
{
  sample.print(os);
  return os;
}

#define BOXM_PLANE_OBS_INSTANTIATE(T) \
template class boxm_plane_obs<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_plane_obs<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_plane_obs<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_plane_obs<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_plane_obs<T > *&);\
template std::ostream& operator << (std::ostream&, boxm_plane_obs<T > const&)

#endif
