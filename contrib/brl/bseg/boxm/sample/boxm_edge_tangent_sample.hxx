#ifndef boxm_edge_tangent_sample_hxx_
#define boxm_edge_tangent_sample_hxx_

#include "boxm_edge_tangent_sample.h"
#include "boxm_plane_obs.h"

template <class OBS_T>
void boxm_edge_tangent_sample<OBS_T>::print(std::ostream& os) const
{
  os << " num_obs=" << this->num_obs() << std::endl;
  for (unsigned i=0; i<obs_.size(); i++)
    os << "    " << i << "- (obs=" << this->obs(i) << ")\n" ;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T> const & sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.num_obs());
  for (unsigned i=0; i<sample.num_obs(); i++)
    vsl_b_write(os, sample.obs(i));
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T> const * &sample)
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_edge_tangent_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream& operator << (std::ostream& os, boxm_edge_tangent_sample<T> &sample)
{
  sample.print(os);
  return os;
}

#define BOXM_EDGE_TANGENT_SAMPLE_INSTANTIATE(T) \
template class boxm_edge_tangent_sample<T >; \
template void vsl_b_write(vsl_b_ostream &, boxm_edge_tangent_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, boxm_edge_tangent_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, boxm_edge_tangent_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, boxm_edge_tangent_sample<T > *&); \
template std::ostream& operator << (std::ostream&, boxm_edge_tangent_sample<T >&)

#endif
