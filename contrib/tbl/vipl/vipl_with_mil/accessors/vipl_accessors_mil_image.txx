#ifndef vipl_accessors_mil_image_txx_
#define vipl_accessors_mil_image_txx_

#include "vipl_accessors_mil_image.h"
#include "../vipl_filterable_section_container_generator_mil_image.txx"

template <class DataType>
DataType fgetpixel(mil_image_2d_of<DataType> const& i, int x, int y, DataType /* dummy */) {
  return i(x,y);
}
template <class DataType>
void fsetpixel(mil_image_2d_of<DataType>& i, int x, int y, DataType e) {
  i(x,y) = e;
}
template <class DataType>
DataType getpixel(mil_image_2d_of<DataType> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || y<0 || x>=i.nx() || y>=i.ny()) return DataType();
  return i(x,y);
}
template <class DataType>
void setpixel(mil_image_2d_of<DataType>& i, int x, int y, DataType e) {
  if (x<0 || y<0 || x>=i.nx() || y>=i.ny()) return;
  i(x,y) = e;
}

#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(mil_image_2d_of<T > const&, int, int, T);\
template void fsetpixel(mil_image_2d_of<T >&, int, int, T);\
template T getpixel(mil_image_2d_of<T > const&, int, int, T);\
template void setpixel(mil_image_2d_of<T >&, int, int, T)

#endif // vipl_accessors_mil_image_txx_
