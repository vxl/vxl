#ifndef vipl_accessors_vil_image_txx_
#define vipl_accessors_vil_image_txx_

#include "vipl_accessors_vil_image.h"
#include <vipl/section/vipl_filterable_section_container_generator_vil_image.txx>

template <class DataType>
DataType fgetpixel(vil_image const& i, int x, int y, DataType /* dummy */) {
  DataType b; i.get_section(&b,x,y,1,1); return b;
}
template <class DataType>
void fsetpixel(vil_image& i, int x, int y, DataType e) {
  i.put_section(&e,x,y,1,1);
}
template <class DataType>
DataType getpixel(vil_image const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || y<0 || x>=i.width() || y>=i.height()) return DataType();
  DataType b; i.get_section(&b,x,y,1,1); return b;
}
template <class DataType>
void setpixel(vil_image& i, int x, int y, DataType e) {
  if (x<0 || y<0 || x>=i.width() || y>=i.height()) return;
  i.put_section(&e,x,y,1,1);
}
// note the (vil_image&)i cast because there is no const vil_image method to get pixels.

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vil_image const&, int, int, T);\
template void fsetpixel(vil_image&, int, int, T);\
template T getpixel(vil_image const&, int, int, T);\
template void setpixel(vil_image&, int, int, T)

#endif // vipl_accessors_vil_image_txx_
