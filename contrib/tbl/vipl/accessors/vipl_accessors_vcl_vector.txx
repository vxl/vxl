#ifndef vipl_accessors_vcl_vector_txx_
#define vipl_accessors_vcl_vector_txx_

#include "vipl_accessors_vcl_vector.h"
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>
#include <vcl_cassert.h>

template <class DataType>
DataType fgetpixel(vcl_vector<DataType> const& i, int x, int y, DataType /* dummy */)
{
  assert(y==0);
  return i[x];
}
template <class DataType>
void fsetpixel(vcl_vector<DataType>& i, int x, int y, DataType e) {
  assert(y==0);
  i[x] = e;
}
template <class DataType>
DataType getpixel(vcl_vector<DataType> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || (unsigned)x>=i.size() || y!=0) return DataType();
  return i[x];
}
template <class DataType>
void setpixel(vcl_vector<DataType>& i, int x, int y, DataType e) {
  if (x<0 || (unsigned)x>=i.size() || y!=0) return;
  i[x] = e;
}

#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vcl_vector<T > const&, int, int, T);\
template void fsetpixel(vcl_vector<T >&, int, int, T);\
template T getpixel(vcl_vector<T > const&, int, int, T);\
template void setpixel(vcl_vector<T >&, int, int, T)

#endif // vipl_accessors_vcl_vector_txx_
