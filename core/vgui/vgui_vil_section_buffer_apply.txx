// This is core/vgui/vgui_vil2_section_buffer_apply.txx
#ifndef vgui_vil2_section_buffer_apply_txx_
#define vgui_vil2_section_buffer_apply_txx_
//:
// \file
// \author Amitha Perera

#include "vgui_vil2_section_buffer_apply.h"
#include "vgui_vil2_section_buffer.h"
#include "internals/vgui_gl_selection_macros.h"

#include <vcl_cassert.h>

namespace
{
  //:
  //
  // Converts the section of \a in marked by (x,y)-(x+w-1,y+h-1) into
  // the output GL buffer \a out.
  //
  // This handles multi-plane images with scalar-valued pixels.
  //
  // This is a helper routine for vgui_vil2_section_buffer_apply
  //
  template<typename InT, typename OutT>
  bool
  convert_buffer( vil2_image_view<InT> const& in,
                  unsigned x, unsigned y, unsigned w, unsigned h,
                  OutT* out, vcl_ptrdiff_t hstep )
  {
    const unsigned ni = in.ni();
    const unsigned nj = in.nj();

    assert( x+w <= ni && y+h <= nj );

    switch( in.nplanes() ) {
      case 1:
      {
        for ( unsigned j=0; j < h; ++j )
          for ( unsigned i=0; i < w; ++i )
            vgui_pixel_convert( in(i+x,j+y), *(out+i+j*hstep) );
        return true;
      }
      case 3:
      {
        for ( unsigned j=0; j < h; ++j )
          for ( unsigned i=0; i < w; ++i )
            vgui_pixel_convert( in(i+x,j+y,0), in(i+x,j+y,1), in(i+x,j+y,2), *(out+i+j*hstep) );
        return true;
      }
      case 4:
      {
        for ( unsigned j=0; j < h; ++j )
          for ( unsigned i=0; i < w; ++i )
            vgui_pixel_convert( in(i+x,j+y,0), in(i+x,j+y,1), in(i+x,j+y,2), in(i+x,j+y,3), *(out+i+j*hstep) );
        return true;
      }
      default:
        return false;
    } // end case
  }
} // end anonymous namespace


//------------------------------------------------------------------------------


template<typename T>
void
vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& sec_buf,
                                vil2_image_view<T> const& image_in )
{
  unsigned x = sec_buf.internal_x();
  unsigned y = sec_buf.internal_y();
  unsigned w = sec_buf.width();
  unsigned h = sec_buf.height();
  unsigned allocw = sec_buf.internal_allocw();
  void* buffer = sec_buf.internal_buffer();
  bool& conversion_okay = sec_buf.internal_conversion_okay();
  GLenum format = sec_buf.internal_format();
  GLenum type = sec_buf.internal_type();

#define Code( BufferType ) \
      conversion_okay = convert_buffer( image_in, x, y, w, h, (BufferType*)buffer, vcl_ptrdiff_t(allocw) );

  ConditionListBegin;
  ConditionListBody( format, type );
  ConditionListFail {
    // shouldn't fail here. If we don't know this format and type, the
    // constructor would've failed.
    assert( false );
  }
#undef Code
}

// There are instantiation problems under MSVC7 because of the
// specializations in the header file. The solution is a tickler
// function. It should not be a static function because the VC7
// optimizer will aggressively remove unused functions, and hence
// will not instantiate.
//

// VC6 Release version has problem with this tickler function.
// For VC6, the solution is to instantiate it explicitly.
#if VCL_VC60
#define INSTANTIATE_VGUI_VIL2_SECTION_BUFFER( T ) \
  template void                                   \
  vgui_vil2_section_buffer_apply( vgui_vil2_section_buffer& sec_buf, \
                                  vil2_image_view<T > const& image_in)

#else
#define INSTANTIATE_VGUI_VIL2_SECTION_BUFFER( T ) \
  void vgui_vil2_section_buffer_tickler( vgui_vil2_section_buffer& sec_buf, vil2_image_view<T >& view ) \
  { \
    vgui_vil2_section_buffer_apply( sec_buf, view ); \
  }
#endif
#endif // vgui_vil2_section_buffer_apply_txx_
