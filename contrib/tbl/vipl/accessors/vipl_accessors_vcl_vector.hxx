#ifndef vipl_accessors_vcl_vector_hxx_
#define vipl_accessors_vcl_vector_hxx_

#include "vipl_accessors_vcl_vector.h"
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.hxx>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class DataType>
DataType fgetpixel(std::vector<DataType> const& i, int x, int y, DataType /* dummy */)
{
  assert(y==0);
  return i[x];
}
template <class DataType>
void fsetpixel(std::vector<DataType>& i, int x, int y, DataType e) {
  assert(y==0);
  i[x] = e;
}
template <class DataType>
DataType getpixel(std::vector<DataType> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || (unsigned)x>=i.size() || y!=0) return DataType();
  return i[x];
}
template <class DataType>
void setpixel(std::vector<DataType>& i, int x, int y, DataType e) {
  assert(x>=0 && y==0);
  if ((unsigned)x>=i.size()) i.resize(x+1,0);
  i[x] = e;
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(std::vector<T > const&, int, int, T);\
template void fsetpixel(std::vector<T >&, int, int, T);\
template T getpixel(std::vector<T > const&, int, int, T);\
template void setpixel(std::vector<T >&, int, int, T)

#endif // vipl_accessors_vcl_vector_hxx_
