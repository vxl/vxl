// This is oxl/vgui/internals/vgui_accelerate_mfc.h
#ifndef vgui_accelerate_mfc_h_
#define vgui_accelerate_mfc_h_
//:
// \file
// \author Marko Bacic, Oxford RRG
// \date   29 Aug 2000
// \brief  Accelerated OpenGL functions for MFC.
//
// \verbatim
//  Modifications:
//    29-AUG-2000 Initial version. Marko Bacic,Oxford RRG
//    04-AUG-2000 Minor fixes. Introduced mb_glDrawBufferWrapper
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/internals/vgui_accelerate.h>

//: Accelerated OpenGL functions for MFC.
//  Provides support for acceleration - fast and smooth glDrawPixels routine
class vgui_accelerate_mfc : public vgui_accelerate
{
 protected:
  char* aux_buffer;
  int aux_buffer_size;
  int BytesPerPixel;

 public:
  //: Constructor.
  vgui_accelerate_mfc();

  //: Destructor.
  virtual ~vgui_accelerate_mfc();

  //: Set the OpenGL cache format.
  virtual bool vgui_choose_cache_format( GLenum* format, GLenum* type);

  //: Fast and smooth glDrawPixels routine for MFC.
  virtual bool vgui_glDrawPixels( GLsizei width, GLsizei height, 
                                  GLenum format, GLenum type, const GLvoid *pixels );

  //: OpenGL clearing.
  virtual bool vgui_glClear(GLbitfield mask );
};

#endif // vgui_accelerate_mfc_h_
