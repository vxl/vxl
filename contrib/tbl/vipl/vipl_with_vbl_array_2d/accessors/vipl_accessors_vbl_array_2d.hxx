#ifndef vipl_accessors_vbl_array_2d_hxx_
#define vipl_accessors_vbl_array_2d_hxx_

#include "vipl_accessors_vbl_array_2d.h"
#include "../vipl_filterable_section_container_generator_vbl_array_2d.hxx"

template <class DataType>
DataType fgetpixel(vbl_array_2d<DataType> const& i, int x, int y, DataType /* dummy */) {
  return i(x,y);
}
template <class DataType>
void fsetpixel(vbl_array_2d<DataType>& i, int x, int y, DataType e) {
  i(x,y) = e;
}
template <class DataType>
DataType getpixel(vbl_array_2d<DataType> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || y<0 || x>=int(i.columns()) || y>=int(i.rows())) return DataType();
  return i(x,y);
}
template <class DataType>
void setpixel(vbl_array_2d<DataType>& i, int x, int y, DataType e) {
  if (x<0 || y<0 || x>=int(i.columns()) || y>=int(i.rows())) return;
  i(x,y) = e;
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vbl_array_2d<T > const&, int, int, T);\
template void fsetpixel(vbl_array_2d<T >&, int, int, T);\
template T getpixel(vbl_array_2d<T > const&, int, int, T);\
template void setpixel(vbl_array_2d<T >&, int, int, T)

#endif // vipl_accessors_vbl_array_2d_hxx_
