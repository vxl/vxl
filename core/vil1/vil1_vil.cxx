// This is core/vil2/vil2_vil1.cxx
#include "vil2_vil1.h"
//:
// \file
// \brief Provide vil2 wrappers of vil1 and vice-versa.
//

#include <vcl_cassert.h>
#include <vil2/vil2_memory_chunk.h>

//: Create a vil2_image_resource from a vil1 vil_image.
vil2_image_resource_sptr vil2_vil1_to_image_resource(const vil_image &vil1_im)
{
  if (!vil1_im) return 0;
  return new vil2_vil1_image_resource(vil1_im);
}

vil2_vil1_image_resource::vil2_vil1_image_resource(const vil_image &src):
src_(src)
{
  // can't cope with images organised RRR..GGG..BBB.. dues to vil_images useless support for planes.
  assert (src_.planes() == 1);
  // can't cope with bool images because vil_image packs the bits.
  assert (src_.bits_per_component() != 1);
}

vil2_pixel_format vil2_vil1_image_resource::pixel_format() const
{
  if (!src_)
    return VIL2_PIXEL_FORMAT_UNKNOWN;

  switch(src_.component_format())
  {
  case VIL_COMPONENT_FORMAT_UNKNOWN:
  case VIL_COMPONENT_FORMAT_COMPLEX:
    return VIL2_PIXEL_FORMAT_UNKNOWN;
  case VIL_COMPONENT_FORMAT_UNSIGNED_INT:
    if (src_.bits_per_component() == 1) return VIL2_PIXEL_FORMAT_BOOL;
    else if (src_.bits_per_component() <= 8) return VIL2_PIXEL_FORMAT_BYTE;
    else if (src_.bits_per_component() <= 16) return VIL2_PIXEL_FORMAT_UINT_16;
    else if (src_.bits_per_component() <= 32) return VIL2_PIXEL_FORMAT_UINT_32;
    else return VIL2_PIXEL_FORMAT_UNKNOWN;
  case VIL_COMPONENT_FORMAT_SIGNED_INT:
    if (src_.bits_per_component() == 1) return VIL2_PIXEL_FORMAT_BOOL;
    else if (src_.bits_per_component() <= 8) return VIL2_PIXEL_FORMAT_SBYTE;
    else if (src_.bits_per_component() <= 16) return VIL2_PIXEL_FORMAT_INT_16;
    else if (src_.bits_per_component() <= 32) return VIL2_PIXEL_FORMAT_INT_32;
    else return VIL2_PIXEL_FORMAT_UNKNOWN;
  case VIL_COMPONENT_FORMAT_IEEE_FLOAT:
    if (src_.bits_per_component() == 32) return VIL2_PIXEL_FORMAT_FLOAT;
    else if (src_.bits_per_component() <= 64) return VIL2_PIXEL_FORMAT_DOUBLE;
    else return VIL2_PIXEL_FORMAT_UNKNOWN;
  default: return VIL2_PIXEL_FORMAT_UNKNOWN;
  }
}


unsigned vil2_vil1_image_resource::ni() const
{
  if (src_)
    return src_.width();
  else
    return 0;
}

unsigned vil2_vil1_image_resource::nj() const
{
  if (src_)
    return src_.height();
  else
    return 0;
}

unsigned vil2_vil1_image_resource::nplanes() const
{
  if (!src_)
    return 0;

  return src_.components();
}

bool vil2_vil1_image_resource::get_property (char const *tag, void *property_value) const
{
  if (src_)
    return src_.get_property(tag, property_value);
  else
    return false;
}

vil2_image_view_base_sptr vil2_vil1_image_resource::get_copy_view(unsigned i0,
                                                                  unsigned ni,
                                                                  unsigned j0,
                                                                  unsigned nj) const
{
  if (!src_)
    return 0;

  switch (pixel_format())
  {
#define macro( F , T ) \
  case F: { \
      vil2_memory_chunk_sptr chunk = new \
        vil2_memory_chunk(ni*nj*nplanes()*sizeof(T ), F ); \
      src_.get_section(chunk->data(), i0, j0, ni, nj); \
      return new vil2_image_view<T >(chunk, (const T *)chunk->data(), \
        ni, nj, nplanes(), nplanes(), ni*nplanes(), 1); }

      macro(VIL2_PIXEL_FORMAT_BYTE , vxl_byte )
      macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
      macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
      macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
      macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
      macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
      macro(VIL2_PIXEL_FORMAT_FLOAT , float )
      macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default: return 0;
  }
}


bool vil2_vil1_image_resource::put_view (const vil2_image_view_base &im, unsigned i0, unsigned j0)
{
  if (!view_fits(im, i0, j0)) return false;
  if (!src_) return false;

  switch (pixel_format())
  {
#define macro( F , T ) \
  case F: { \
      const vil2_image_view<T > &view = \
        static_cast<const vil2_image_view<T > &>(im); \
      assert(nplanes()==1 || view.planestep() == 1); \
      src_.put_section(view.top_left_ptr(), i0, j0, im.ni(), im.nj()); }

      macro(VIL2_PIXEL_FORMAT_BYTE , vxl_byte )
      macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
      macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
      macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
      macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
      macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
      macro(VIL2_PIXEL_FORMAT_FLOAT , float )
      macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default: return false;
  }
}
