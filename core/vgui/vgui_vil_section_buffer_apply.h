#ifndef VGUI_VIL2_SECTION_BUFFER_APPLY_H_
#define VGUI_VIL2_SECTION_BUFFER_APPLY_H_

//:
// \file
// \author Amitha Perera
// \brief  Populates a vgui_vil2_section_buffer using a vil2_image_view.
//
// This function is implemented as a non-member function because
// member templates are not supported by all compilers.

#include <vil2/vil2_fwd.h>
class vgui_vil2_section_buffer;

//: Grab a GL section from the given image.
//
// This caters for multi-planed images with scalar valued pixels.
//
// \relates vgui_vil2_section_buffer
template<typename T>
void
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& buf,
                                vil2_image_view<T> const& );


//: Grab a GL section from the given image.
//
// This caters for single-planed images with RGB pixels. It simply
// calls the other version with a multi-plane view of the same data.
//
// \relates vgui_vil2_section_buffer
template<typename T>
void
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& sec_buf,
                                vil2_image_view< vil2_rgb<T> > const& image_in );

//: Grab a GL section from the given image.
//
// This caters for single-planed images with RGBA pixels. It simply
// calls the other version with a multi-plane view of the same data.
//
// \relates vgui_vil2_section_buffer
template<typename T>
void
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& sec_buf,
                                vil2_image_view< vil2_rgba<T> > const& image_in );

#endif // VGUI_VIL2_SECTION_BUFFER_APPLY_H_
