// This is ./oxl/vgui/internals/vgui_accelerate_mfc.h

//:
// \file
// \author Marko Bacic, Oxford RRG
// \date   29 Aug 2000
// \brief  Provides support for acceleration - fast and smooth glDrawPixels routine.
//
// \verbatim
//  Modifications:
//    29-AUG-2000 Initial version. Marko Bacic,Oxford RRG
//    04-AUG-2000 Minor fixes. Introduced mb_glDrawBufferWrapper
// \endverbatim

#ifndef vgui_accelerate_mfc_h_
#define vgui_accelerate_mfc_h_

#include <vgui/vgui_gl.h>
#include <vgui/internals/vgui_accelerate.h>

//: Provides support for acceleration - fast and smooth glDrawPixels routine
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
