#ifndef vipl_accessors_section_hxx_
#define vipl_accessors_section_hxx_

#include "vipl_accessors_section.h"
#include "../vipl_filterable_section_container_generator_section.hxx"

template <class DataType>
DataType fgetpixel(section<DataType,2> const& i, int x, int y, DataType /* dummy */) {
  return i.Value(x,y);
}
template <class DataType>
void fsetpixel(section<DataType,2>& i, int x, int y, DataType e) {
  i.Set(e,x,y);
}
template <class DataType>
DataType getpixel(section<DataType,2> const& i, int x, int y, DataType /* dummy */) {
  if (x<0 || y<0 || x>=int(i.Size(0)) || y>=int(i.Size(1))) return DataType();
  return i.Value(x,y);
}
template <class DataType>
void setpixel(section<DataType,2>& i, int x, int y, DataType e) {
  if (x<0 || y<0 || x>=int(i.Size(0)) || y>=int(i.Size(1))) return;
  i.Set(e,x,y);
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(section<T,2> const&, int, int, T);\
template void fsetpixel(section<T,2>&, int, int, T);\
template T getpixel(section<T,2> const&, int, int, T);\
template void setpixel(section<T,2>&, int, int, T)

#endif // vipl_accessors_section_hxx_
