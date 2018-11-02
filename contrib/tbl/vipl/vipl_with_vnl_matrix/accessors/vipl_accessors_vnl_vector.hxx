#ifndef vipl_accessors_vnl_vector_hxx_
#define vipl_accessors_vnl_vector_hxx_

#include "vipl_accessors_vnl_vector.h"
#include "../vipl_filterable_section_container_generator_vnl_vector.hxx"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class DataType>
DataType fgetpixel(vnl_vector<DataType> const& i, int x, int y, DataType /* dummy */)
{
  assert(y==0);
  return i(x);
}
template <class DataType>
void fsetpixel(vnl_vector<DataType>& i, int x, int y, DataType e) {
  assert(y==0);
  i(x) = e;
}
template <class DataType>
DataType getpixel(vnl_vector<DataType> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || (unsigned)x>=i.size() || y!=0) return DataType();
  return i(x);
}
template <class DataType>
void setpixel(vnl_vector<DataType>& i, int x, int y, DataType e) {
  if (x<0 || (unsigned)x>=i.size() || y!=0) return;
  i(x) = e;
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vnl_vector<T > const&, int, int, T);\
template void fsetpixel(vnl_vector<T >&, int, int, T);\
template T getpixel(vnl_vector<T > const&, int, int, T);\
template void setpixel(vnl_vector<T >&, int, int, T)

#endif // vipl_accessors_vnl_vector_hxx_
