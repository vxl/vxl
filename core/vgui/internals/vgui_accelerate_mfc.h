#ifndef vgui_accelerate_mfc_h_
#define vgui_accelerate_mfc_h_
//
// .NAME vgui_accelerate_mfc - Provides support for acceleration
// .LIBRARY vgui
// .HEADER vgui Package
// .INCLUDE vgui/internals/vgui_accelerate_mfc.h
// .FILE vgui_accelerate_mfc.cxx
//
// .SECTION Description:
//    Fast and smooth glDrawPixels routine
//
// .SECTION Author:
//    29-AUG-2000 Initial version. Marko Bacic,Oxford RRG
//
// .SECTION Modifications:
//    04-AUG-2000 Minor fixes. Introduced mb_glDrawBufferWrapper
//-----------------------------------------------------------------------------

#include <vgui/vgui_gl.h>
#include <vgui/internals/vgui_accelerate.h>


class vgui_accelerate_mfc : public vgui_accelerate {
public:
  vgui_accelerate_mfc();
  virtual ~vgui_accelerate_mfc();

  virtual bool vgui_choose_cache_format( GLenum* format, GLenum* type);
  virtual bool vgui_glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
  virtual bool vgui_glClear(GLbitfield mask );
protected:
  char* aux_buffer;
  int aux_buffer_size;
  int BytesPerPixel;
};

#endif // vgui_accelerate_mfc_h_
