// This is ./oxl/vgui/internals/vgui_accelerate.h

//:
// \file
// \author David Capel, Robotics Research Group, University of Oxford
// \date   1 April 2000
// \brief  (Potentially) Accelerated functions
//
// \verbatim
//  Modifications
//   01-APR-2000   initial version
//   10-JUL-2000   Marko Bacic, Oxford RRG - Speeded up rendering on Mesa by using GL_RGB instead of GL_RGBA
//   Feb.2002    - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#ifndef vgui_accelerate_h_
#define vgui_accelerate_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vgui/vgui_gl.h>

//: (Potentially) Accelerated functions
class vgui_accelerate {
public:
  static bool vgui_no_acceleration;
  static bool vgui_mfc_acceleration;

  static vgui_accelerate* instance();

  virtual ~vgui_accelerate();

  virtual bool vgui_glClear( GLbitfield mask );

  //:
  // If you pass stuff to vgui_glDrawPixels, and the format and type are what you
  // got from an earlier call to this baby, then it might go faster.
  virtual bool vgui_choose_cache_format( GLenum* format, GLenum* type);

  //: Fast-as-we-can version of drawpixels.
  virtual bool vgui_glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );

  virtual bool vgui_copy_back_to_aux();
  virtual bool vgui_copy_aux_to_back();

  static void register_accelerator (vgui_accelerate* p, int level);

protected:
  vgui_accelerate();
};

#endif // vgui_accelerate_h_
