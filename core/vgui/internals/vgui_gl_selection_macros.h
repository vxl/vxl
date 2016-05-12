#ifndef VGUI_GL_SELECTION_MACROS_H_
#define VGUI_GL_SELECTION_MACROS_H_
//:
// \file
// \author Amitha Perera
//
// This collection of macros is used to simplify the selection of the
// vgui_pixel_* type based on the GL format and GL type. Suppose the
// current format is in fmt and the current type is in typ. First
// define a macro "Code" that containing the code that relies on the
// corresponding vgui_pixel_type, and then call ConditionListBegin,
// ConditionListBody, ConditionListFail. This will generate a
// collection of if statements that will run "Code" with the
// appropriate type. For example:
//
// \code
// #define Code(pixel_type) buffer_of<pixel_type > buffer; convert_to_buffer(in_image,buffer);
//
//    ConditionListBegin;
//    ConditionListBody( fmt, typ );
//    ConditionListFail
//    {
//       std::cerr << "don't know " << fmt << " and " << typ << "\n";
//       return false;
//    }
//
// #undef Code
// \endcode
//
// will generate code like
//
// \code
// if ( fmt==GL_RGB && typ==GL_UNSIGNED )
// {
//   buffer_of<vgui_pixel_rgb888 > buffer;
//   convert_to_buffer(in_image,buffer);
// } else if ( fmt==GL_RGB && typ==GL_UNSIGNED_SHORT_5_6_5 )
// {
//   buffer_of<vgui_pixel_rgb565 > buffer;
//   convert_to_buffer(in_image,buffer);
// } else if (
//      ...
// } else
// {
//   std::cerr << "don't know " << fmt << " and " << typ << "\n";
//   return false;
// }
// \endcode
//
// If you don't want to handle the failure condition, you can replace
// ConditionListFail with ConditionListEnd.

#include <vgui/vgui_gl.h>
#include <vgui/vgui_pixel.h>

#define ConditionList0( format, type ) \
  else if ( format==GL_RGB && type==GL_UNSIGNED_BYTE ) { Code( vgui_pixel_rgb888 ) } \
  else if ( format==GL_BGR && type==GL_UNSIGNED_BYTE ) { Code( vgui_pixel_bgr888 ) } \
  else if ( format==GL_RGBA && type==GL_UNSIGNED_BYTE ) { Code( vgui_pixel_rgba8888 ) }

#if defined(GL_UNSIGNED_SHORT_5_6_5)
#define ConditionList1( format, type ) \
  else if ( format==GL_RGB && type==GL_UNSIGNED_SHORT_5_6_5 ) { Code( vgui_pixel_rgb565 ) }
#else
#define ConditionList1( format, type ) /* null */
#endif

// Is this right? GL_RGB and pixel_bgra? It is missing some endian issues?
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
#define ConditionList2( format, type ) \
  else if ( format==GL_RGB && type==GL_UNSIGNED_SHORT_5_5_5_1 ) { Code( vgui_pixel_bgra5551 ) }
#else
#define ConditionList2( format, type ) /* null */
#endif

#if defined(GL_BGRA)
#define ConditionList3( format, type ) \
  else if ( format==GL_BGRA && type==GL_UNSIGNED_BYTE ) { Code( vgui_pixel_bgra8888 ) }
#else
#define ConditionList3( format, type ) /* null */
#endif

#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
#define ConditionList4( format, type ) \
  else if ( format==GL_ABGR_EXT && type==GL_UNSIGNED_BYTE ) { Code( vgui_pixel_abgr8888 ) }
#else
#define ConditionList4( format, type ) /* null */
#endif


#define ConditionListBody( format, type ) \
  ConditionList0(format,type) \
  ConditionList1(format,type) \
  ConditionList2(format,type) \
  ConditionList3(format,type) \
  ConditionList4(format,type) \
  else if (0)do{}while (0)

#define ConditionListBegin if (0) do {} while (0)
#define ConditionListFail else
#define ConditionListEnd  else { /* null */ }

#endif // VGUI_GL_SELECTION_MACROS_H_
