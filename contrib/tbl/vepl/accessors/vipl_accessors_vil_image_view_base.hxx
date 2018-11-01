#ifndef vipl_accessors_vil_image_view_base_hxx_
#define vipl_accessors_vil_image_view_base_hxx_

#include "vipl_accessors_vil_image_view_base.h"
#include <vepl/section/vipl_filterable_section_container_generator_vil_image_view_base.hxx>
#include <vil/vil_rgb.h>
#include <vil/vil_image_view.h>

template <class DataType>
inline DataType fgetpixel(vil_image_view_base const& i, int x, int y, DataType /* dummy */)
{
  vil_image_view<DataType> const& im = static_cast<vil_image_view<DataType>const&>(i);
  return im(x,y);
}
template <class DataType>
inline void fsetpixel(vil_image_view_base& i, int x, int y, DataType const e)
{
  vil_image_view<DataType>& im = static_cast<vil_image_view<DataType>&>(i);
  im(x,y) = e;
}
template <class DataType>
inline DataType getpixel(vil_image_view_base const& i, int x, int y, DataType /* dummy */)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return DataType();
  vil_image_view<DataType> const& im = static_cast<vil_image_view<DataType>const&>(i);
  return im(x,y);
}
template <class DataType>
inline void setpixel(vil_image_view_base& i, int x, int y, DataType const e)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return;
  vil_image_view<DataType>& im = static_cast<vil_image_view<DataType>&>(i);
  im(x,y) = e;
}

template <class DataType>
inline vil_rgb<DataType> fgetpixel(vil_image_view_base const& i, int x, int y, vil_rgb<DataType> /* dummy */)
{
  if (i.nplanes() == 3)
  {
    vil_image_view<DataType> const& im = static_cast<vil_image_view<DataType>const&>(i);
    return vil_rgb<DataType>(im(x,y,0),im(x,y,1),im(x,y,2));
  }
  else
  {
    vil_image_view<vil_rgb<DataType> > const& im = static_cast<vil_image_view<vil_rgb<DataType> >const&>(i);
    return im(x,y);
  }
}
template <class DataType>
inline void fsetpixel(vil_image_view_base& i, int x, int y, vil_rgb<DataType> const e)
{
  if (i.nplanes() == 3)
  {
    vil_image_view<DataType>& im = static_cast<vil_image_view<DataType>&>(i);
    im(x,y,0) = e.r; im(x,y,1) = e.g; im(x,y,2) = e.b;
  }
  else
  {
    vil_image_view<vil_rgb<DataType> >& im = static_cast<vil_image_view<vil_rgb<DataType> >&>(i);
    im(x,y) = e;
  }
}
template <class DataType>
inline vil_rgb<DataType> getpixel(vil_image_view_base const& i, int x, int y, vil_rgb<DataType> /* dummy */)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return vil_rgb<DataType>();
  if (i.nplanes() == 3)
  {
    vil_image_view<DataType> const& im = static_cast<vil_image_view<DataType>const&>(i);
    return vil_rgb<DataType>(im(x,y,0),im(x,y,1),im(x,y,2));
  }
  else
  {
    vil_image_view<vil_rgb<DataType> > const& im = static_cast<vil_image_view<vil_rgb<DataType> >const&>(i);
    return im(x,y);
  }
}
template <class DataType>
inline void setpixel(vil_image_view_base& i, int x, int y, vil_rgb<DataType> const e)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return;
  if (i.nplanes() == 3)
  {
    vil_image_view<DataType>& im = static_cast<vil_image_view<DataType>&>(i);
    im(x,y,0) = e.r; im(x,y,1) = e.g; im(x,y,2) = e.b;
  }
  else
  {
    vil_image_view<vil_rgb<DataType> >& im = static_cast<vil_image_view<vil_rgb<DataType> >&>(i);
    im(x,y) = e;
  }
}

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vil_image_view_base const&, int, int, T);\
template void fsetpixel(vil_image_view_base&, int, int, T);\
template T getpixel(vil_image_view_base const&, int, int, T);\
template void setpixel(vil_image_view_base&, int, int, T)

#endif // vipl_accessors_vil_image_view_base_hxx_
