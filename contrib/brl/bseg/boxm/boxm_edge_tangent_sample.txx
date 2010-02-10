#ifndef boxm_edge_tangent_sample_txx_
#define boxm_edge_tangent_sample_txx_

#include "boxm_edge_tangent_sample.h"

#include <vgl/io/vgl_io_homg_plane_3d.h>
#include <vgl/io/vgl_io_infinite_line_3d.h>

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
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_edge_tangent_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_plane_obs<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_plane_obs<T>& sample)
{
  sample.print(os);
  return os;
}

// Edge Tangent Sample Methods
template <class OBS_T>
void boxm_edge_tangent_sample<OBS_T>::print(vcl_ostream& os)
{
  for (unsigned i=0; i<obs_.size(); i++) 
    os << i << "(obs=" << this->obs(i) << " num_obs=" << this->num_obs() << ")\n" ;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T> & sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.num_obs());
  for (unsigned i=0; i<sample.num_obs(); i++) 
    vsl_b_write(os, sample.obs(i));
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T>* &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
     { unsigned num;
      vsl_b_read(is, num);
      boxm_plane_obs<T> o;
      for (unsigned i=0; i<num; i++) {
        vsl_b_read(is, o);
        sample.insert(o);
      }}
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_edge_tangent_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_edge_tangent_sample<T> sample)
{
  sample.print(os);
  return os;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.line_);
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
      vsl_b_read(is, sample.line_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_edge_tangent_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_inf_line_sample<T>& sample)
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
template vcl_ostream& operator << (vcl_ostream&, boxm_plane_obs<T >&)

#define BOXM_EDGE_TANGENT_SAMPLE_INSTANTIATE(T) \
template class boxm_edge_tangent_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_edge_tangent_sample<T >&); \
template void vsl_b_write(vsl_b_ostream &, boxm_edge_tangent_sample<T >*&); \
template void vsl_b_read(vsl_b_istream &, boxm_edge_tangent_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_edge_tangent_sample<T > *&); \
template vcl_ostream& operator << (vcl_ostream&, boxm_edge_tangent_sample<T >)

#define BOXM_INF_LINE_SAMPLE_INSTANTIATE(T) \
template class boxm_inf_line_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_inf_line_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_inf_line_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_inf_line_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_inf_line_sample<T > *&);\
template vcl_ostream& operator << (vcl_ostream&, boxm_inf_line_sample<T >&)

#endif
