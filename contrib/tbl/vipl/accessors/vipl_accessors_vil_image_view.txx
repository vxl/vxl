#ifndef vipl_accessors_vil_image_view_txx_
#define vipl_accessors_vil_image_view_txx_

#include "vipl_accessors_vil_image_view.h"
#include <vipl/section/vipl_filterable_section_container_generator_vil_image_view.txx>

template <class DataType>
DataType fgetpixel(vil_image_view<DataType> const& i, int x, int y, DataType /* dummy */)
{
  return i(x,y);
}

template <class DataType>
void fsetpixel(vil_image_view<DataType>& i, int x, int y, DataType e)
{
  i(x,y) = e;
}

template <class DataType>
DataType getpixel(vil_image_view<DataType> const& i, int x, int y, DataType /* dummy */)
{
  if (x<0 || y<0 || x>=int(i.ni()) || y>=int(i.nj())) return DataType();
  return i(x,y);
}

template <class DataType>
void setpixel(vil_image_view<DataType>& i, int x, int y, DataType e)
{
  if (x<0 || y<0 || x>=int(i.ni()) || y>=int(i.nj())) return;
  i(x,y) = e;
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vil_image_view<T > const&, int, int, T);\
template void fsetpixel(vil_image_view<T >&, int, int, T);\
template T getpixel(vil_image_view<T > const&, int, int, T);\
template void setpixel(vil_image_view<T >&, int, int, T)

#endif // vipl_accessors_vil_image_view_txx_
