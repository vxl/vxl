// This is core/vgui/vgui_vil2_section_buffer_apply.h
#ifndef VGUI_VIL2_SECTION_BUFFER_APPLY_H_
#define VGUI_VIL2_SECTION_BUFFER_APPLY_H_
//:
// \file
// \author Amitha Perera
// \brief  Populates a vgui_vil2_section_buffer using a vil2_image_view.
//
// This function is implemented as a non-member function because
// member templates are not supported by all compilers.

#include <vxl_config.h> // for vxl_byte etc.
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_rgb.h>
#include <vil2/vil2_rgba.h>
class vgui_vil2_section_buffer;

//: Grab a GL section from the given image.
//
// This caters for multi-planed images with scalar valued pixels, when T
// is non-composite, or for single-planed images with RGB or RGBA pixels,
// in the case where T is vil2_rgb<U> or vil2_rgba<U>.  In that case, it
// simply calls the other version with a multi-plane view of the same data.
//
// \relates vgui_vil2_section_buffer
template<typename T>
void
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& buf,
                                vil2_image_view<T> const& );

#define sba_macro(T) \
VCL_DEFINE_SPECIALIZATION void \
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& sec_buf, \
                                vil2_image_view< vil2_rgb<T> > const& image_in ) \
{ \
  vil2_image_view<T> planes_view = image_in; \
  vgui_vil2_section_buffer_apply( sec_buf, planes_view ); \
} \
VCL_DEFINE_SPECIALIZATION void \
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& sec_buf, \
                                vil2_image_view< vil2_rgba<T> > const& image_in ) \
{ \
  vil2_image_view<T> planes_view = image_in; \
  vgui_vil2_section_buffer_apply( sec_buf, planes_view ); \
}

sba_macro(vxl_byte)
sba_macro(vxl_sbyte)
sba_macro(vxl_int_16)
sba_macro(vxl_int_32)
sba_macro(vxl_uint_16)
sba_macro(vxl_uint_32)
sba_macro(float)
sba_macro(double)

#undef sba_macro

#endif // VGUI_VIL2_SECTION_BUFFER_APPLY_H_
