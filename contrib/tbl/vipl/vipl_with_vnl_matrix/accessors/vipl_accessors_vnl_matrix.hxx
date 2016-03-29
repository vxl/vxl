#ifndef vipl_accessors_vnl_matrix_hxx_
#define vipl_accessors_vnl_matrix_hxx_

#include "vipl_accessors_vnl_matrix.h"
#include "../vipl_filterable_section_container_generator_vnl_matrix.hxx"

template <class DataType>
DataType fgetpixel(vnl_matrix<DataType> const& i, int x, int y, DataType /* dummy */) {
  return i(x,y);
}
template <class DataType>
void fsetpixel(vnl_matrix<DataType>& i, int x, int y, DataType e) {
  i(x,y) = e;
}
template <class DataType>
DataType getpixel(vnl_matrix<DataType> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || y<0 || (unsigned)x>=i.columns() || (unsigned)y>=i.rows()) return DataType();
  return i(x,y);
}
template <class DataType>
void setpixel(vnl_matrix<DataType>& i, int x, int y, DataType e) {
  if (x<0 || y<0 || (unsigned)x>=i.columns() || (unsigned)y>=i.rows()) return;
  i(x,y) = e;
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vnl_matrix<T > const&, int, int, T);\
template void fsetpixel(vnl_matrix<T >&, int, int, T);\
template T getpixel(vnl_matrix<T > const&, int, int, T);\
template void setpixel(vnl_matrix<T >&, int, int, T)

#endif // vipl_accessors_vnl_matrix_hxx_
