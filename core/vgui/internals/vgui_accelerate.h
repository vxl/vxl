// This is core/vgui/internals/vgui_accelerate.h
#ifndef vgui_accelerate_h_
#define vgui_accelerate_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
//   July 2003   - Mark Johnson - set vgui_mvc_acceleration to false by default; conflicts with overlays
// \endverbatim

#include <vgui/vgui_gl.h>

//: (Potentially) Accelerated functions for OpenGL.
class vgui_accelerate
{
 public:
  //: Set to true for no acceleration.
  static bool vgui_no_acceleration;

  //: Set to true to use MFC acceleration.
  static bool vgui_mfc_acceleration;

  //: Sometimes double buffering needs to be switched off on windows.
  static bool vgui_doublebuffer;

  //: Singleton instance of this class.
  static vgui_accelerate* instance();

  //: Destructor.
  virtual ~vgui_accelerate() {}

  //: OpenGL clearing.
  virtual bool vgui_glClear( GLbitfield mask );

  //: Set the OpenGL cache format.
  //  If you pass stuff to vgui_glDrawPixels, and the format and type are what
  //  you got from an earlier call to this baby, then it might go faster.
  virtual bool vgui_choose_cache_format( GLenum* format, GLenum* type);

  //: Fast-as-we-can version of drawpixels.
  virtual bool vgui_glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );

  //: For X11/Mesa - copy back buffer to auxiliary buffer.
  //  This function is used in X11/Mesa to speed up overlay emulation.  It
  //  returns false to indicate to overlay_biscuit that a default emulation
  //  must be used.
  virtual bool vgui_copy_back_to_aux();

  //: For X11/Mesa - copy auxiliary buffer to back buffer.
  //  This function is used in X11/Mesa to speed up overlay emulation.  It
  //  returns false to indicate to overlay_biscuit that a default emulation
  //  must be used.
  virtual bool vgui_copy_aux_to_back();

  //: If level is higher than current level, then change to given accelerator.
  static void register_accelerator (vgui_accelerate* p, int level);

 protected:
  vgui_accelerate() {}
};

#endif // vgui_accelerate_h_
