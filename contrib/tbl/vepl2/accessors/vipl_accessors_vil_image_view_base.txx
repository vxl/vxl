#ifndef vipl_accessors_vil2_image_view_base_txx_
#define vipl_accessors_vil2_image_view_base_txx_

#include "vipl_accessors_vil2_image_view_base.h"
#include <vepl2/section/vipl_filterable_section_container_generator_vil2_image_view_base.txx>
#include <vil/vil_rgb.h>
#include <vcl_config_compiler.h>

template <class DataType>
inline DataType fgetpixel(vil2_image_view_base const& i, int x, int y, DataType /* dummy */)
{
  vil2_image_view<DataType> const& im = static_cast<vil2_image_view<DataType>const&>(i);
  return im(x,y);
}
template <class DataType>
inline void fsetpixel(vil2_image_view_base& i, int x, int y, DataType e)
{
  vil2_image_view<DataType>& im = static_cast<vil2_image_view<DataType>&>(i);
  im(x,y) = e;
}
template <class DataType>
inline DataType getpixel(vil2_image_view_base const& i, int x, int y, DataType /* dummy */)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return DataType();
  vil2_image_view<DataType> const& im = static_cast<vil2_image_view<DataType>const&>(i);
  return im(x,y);
}
template <class DataType>
inline void setpixel(vil2_image_view_base& i, int x, int y, DataType e)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return;
  vil2_image_view<DataType>& im = static_cast<vil2_image_view<DataType>&>(i);
  im(x,y) = e;
}

#if VCL_HAS_TEMPLATE_SYMBOLS
template <class DataType>
inline vil_rgb<DataType> fgetpixel(vil2_image_view_base const& i, int x, int y, vil_rgb<DataType> /* dummy */)
{
  if (i.nplanes() == 3)
  {
    vil2_image_view<DataType> const& im = static_cast<vil2_image_view<DataType>const&>(i);
    return vil_rgb<DataType>(im(x,y,0),im(x,y,1),im(x,y,2));
  }
  else
  {
    vil2_image_view<vil_rgb<DataType> > const& im = static_cast<vil2_image_view<vil_rgb<DataType> >const&>(i);
    return im(x,y);
  }
}
template <class DataType>
inline void fsetpixel(vil2_image_view_base& i, int x, int y, vil_rgb<DataType> e)
{
  if (i.nplanes() == 3)
  {
    vil2_image_view<DataType>& im = static_cast<vil2_image_view<DataType>&>(i);
    im(x,y,0) = e.r; im(x,y,1) = e.g; im(x,y,2) = e.b;
  }
  else
  {
    vil2_image_view<vil_rgb<DataType> >& im = static_cast<vil2_image_view<vil_rgb<DataType> >&>(i);
    im(x,y) = e;
  }
}
template <class DataType>
inline vil_rgb<DataType> getpixel(vil2_image_view_base const& i, int x, int y, vil_rgb<DataType> /* dummy */)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return vil_rgb<DataType>();
  if (i.nplanes() == 3)
  {
    vil2_image_view<DataType> const& im = static_cast<vil2_image_view<DataType>const&>(i);
    return vil_rgb<DataType>(im(x,y,0),im(x,y,1),im(x,y,2));
  }
  else
  {
    vil2_image_view<vil_rgb<DataType> > const& im = static_cast<vil2_image_view<vil_rgb<DataType> >const&>(i);
    return im(x,y);
  }
}
template <class DataType>
inline void setpixel(vil2_image_view_base& i, int x, int y, vil_rgb<DataType> e)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return;
  if (i.nplanes() == 3)
  {
    vil2_image_view<DataType>& im = static_cast<vil2_image_view<DataType>&>(i);
    im(x,y,0) = e.r; im(x,y,1) = e.g; im(x,y,2) = e.b;
  }
  else
  {
    vil2_image_view<vil_rgb<DataType> >& im = static_cast<vil2_image_view<vil_rgb<DataType> >&>(i);
    im(x,y) = e;
  }
}
#else // compilers with overload problems ...
inline vil_rgb<vxl_byte> fgetpixel(vil2_image_view_base const& i, int x, int y, vil_rgb<vxl_byte> /* dummy */)
{
  if (i.nplanes() == 3)
  {
    vil2_image_view<vxl_byte> const& im = static_cast<vil2_image_view<vxl_byte>const&>(i);
    return vil_rgb<vxl_byte>(im(x,y,0),im(x,y,1),im(x,y,2));
  }
  else
  {
    vil2_image_view<vil_rgb<vxl_byte> > const& im = static_cast<vil2_image_view<vil_rgb<vxl_byte> >const&>(i);
    return im(x,y);
  }
}
inline void fsetpixel(vil2_image_view_base& i, int x, int y, vil_rgb<vxl_byte> e)
{
  if (i.nplanes() == 3)
  {
    vil2_image_view<vxl_byte>& im = static_cast<vil2_image_view<vxl_byte>&>(i);
    im(x,y,0) = e.r; im(x,y,1) = e.g; im(x,y,2) = e.b;
  }
  else
  {
    vil2_image_view<vil_rgb<vxl_byte> >& im = static_cast<vil2_image_view<vil_rgb<vxl_byte> >&>(i);
    im(x,y) = e;
  }
}
inline vil_rgb<vxl_byte> getpixel(vil2_image_view_base const& i, int x, int y, vil_rgb<vxl_byte> /* dummy */)
{
  if (x<0 || y<0 || x>=(int)i.ni() || y>=(int)i.nj()) return vil_rgb<vxl_byte>();
  if (i.nplanes() == 3)
  {
    vil2_image_view<vxl_byte> const& im = static_cast<vil2_image_view<vxl_byte>const&>(i);
    return vil_rgb<vxl_byte>(im(x,y,0),im(x,y,1),im(x,y,2));
  }
  else
  {
    vil2_image_view<vil_rgb<vxl_byte> > const& im = static_cast<vil2_image_view<vil_rgb<vxl_byte> >const&>(i);
    return im(x,y);
  }
}
inline void setpixel(vil2_image_view_base& i, int x, int y, vil_rgb<vxl_byte> e)
{
  if (x<0 || y<0 || x>=i.ni() || y>=i.nj()) return;
  if (i.nplanes() == 3)
  {
    vil2_image_view<vxl_byte>& im = static_cast<vil2_image_view<vxl_byte>&>(i);
    im(x,y,0) = e.r; im(x,y,1) = e.g; im(x,y,2) = e.b;
  }
  else
  {
    vil2_image_view<vil_rgb<vxl_byte> >& im = static_cast<vil2_image_view<vil_rgb<vxl_byte> >&>(i);
    im(x,y) = e;
  }
}
#endif

#undef VIPL_INSTANTIATE_ACCESSORS
#define VIPL_INSTANTIATE_ACCESSORS(T) \
template T fgetpixel(vil2_image_view_base const&, int, int, T);\
template void fsetpixel(vil2_image_view_base&, int, int, T);\
template T getpixel(vil2_image_view_base const&, int, int, T);\
template void setpixel(vil2_image_view_base&, int, int, T)

#endif // vipl_accessors_vil2_image_view_base_txx_
