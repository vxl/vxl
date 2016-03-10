#ifndef bvpl_octree_sample_hxx_
#define bvpl_octree_sample_hxx_

#include "bvpl_octree_sample.h"


template<class T_data>
void vsl_b_write(vsl_b_ostream & os, bvpl_octree_sample<T_data> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.response_);
}

template<class T_data>
void vsl_b_write(vsl_b_ostream & os, bvpl_octree_sample<T_data> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template<class T_data>
void vsl_b_read(vsl_b_istream & is, bvpl_octree_sample<T_data> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.response_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template<class T_data>
void vsl_b_read(vsl_b_istream & is, bvpl_octree_sample<T_data> *&sample)
{
  vsl_b_read(is, *sample);
}


template<class T_data>
void bvpl_octree_sample<T_data>::print(std::ostream& os) const
{
  os << "(response=" << response_ << " id=" << id_ <<')';
}

template<class T_data>
std::ostream& operator << (std::ostream& os, const bvpl_octree_sample<T_data>& sample)
{
  sample.print(os);
  return os;
}

#define BVPL_OCTREE_SAMPLE_INSTANTIATE(T) \
template class bvpl_octree_sample<T >; \
template std::ostream& operator << (std::ostream&, const bvpl_octree_sample<T >&); \
template void vsl_b_write(vsl_b_ostream &, bvpl_octree_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, bvpl_octree_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, bvpl_octree_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, bvpl_octree_sample<T > *&)

#endif
