// This is core/vgui/internals/vgui_accelerate.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author David Capel
// \date   01 Apr 2000
// \brief  See vgui_accelerate.h for a description of this file.

#ifdef HAS_MFC
#include <vgui/impl/mfc/StdAfx.h>
extern CDC *vgui_mfc_adaptor_global_dc;
#endif

#include "vgui_accelerate.h"

#include <vgui/vgui_gl.h>

bool vgui_accelerate::vgui_no_acceleration = false;
bool vgui_accelerate::vgui_mfc_acceleration = false;
bool vgui_accelerate::vgui_doublebuffer = true;

static int accelerator_level = 0;
static vgui_accelerate* vgui_accelerator = 0;
vgui_accelerate* vgui_accelerate::instance()
{
  if (!vgui_accelerator) vgui_accelerator = new vgui_accelerate;
  return vgui_accelerator;
}

void vgui_accelerate::register_accelerator(vgui_accelerate* p, int level)
{
  if (level > accelerator_level) {
    delete vgui_accelerator;
    vgui_accelerator = p;
    accelerator_level = level;
  }
}

// Default implementations (return false to indicate that a non-accelerated path was used.)

bool
vgui_accelerate::vgui_glClear( GLbitfield mask )
{
  glClear(mask);
  return false;
}

bool
vgui_accelerate::vgui_glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  glDrawPixels( width, height, format, type, pixels);
  return false;
}

// 32 bit RGBA seems to be acceptable/fast on most platforms.
// - u97mb RGBA is not acceptable on Mesa(too slow) so we use GL_RGB instead
bool
vgui_accelerate::vgui_choose_cache_format( GLenum* format, GLenum* type)
{
#if VGUI_MESA
  (*format) = GL_RGB;
#else
  (*format) = GL_RGBA;
#endif
  (*type) = GL_UNSIGNED_BYTE;
  return false;
}

// These functions are used in X11/Mesa to speed up overlay emulation. They
// return false to indicate to overlay_biscuit that a default emulation must be used.
bool vgui_accelerate::vgui_copy_back_to_aux()
{
  return false;
}

bool vgui_accelerate::vgui_copy_aux_to_back()
{
  return false;
}
