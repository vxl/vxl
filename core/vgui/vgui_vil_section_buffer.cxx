// This is core/vgui/vgui_vil2_section_buffer.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil2_section_buffer.h for a description of this file.
//
// cut-n-paste and modify from vgui_section_buffer.cxx

#include "vgui_vil2_section_buffer.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vgui/vgui_macro.h>
#include <vgui/vgui_section_render.h>
#include <vgui/internals/vgui_accelerate.h>

#include "internals/vgui_gl_selection_macros.h"


//------------------------------------------------------------------------------

//: Constructor.
//
// Determine the size of the buffer and allocate it.
//
vgui_vil2_section_buffer::
vgui_vil2_section_buffer( unsigned x, unsigned y,
                          unsigned w, unsigned h,
                          GLenum format,
                          GLenum type )
  : format_(format), type_(type),
    x_(x), y_(y), w_(w), h_(h)
{
  assert( w_ > 0 && h_ > 0 );

  // no texture support yet, so allocate just what we need.
  allocw_ = w_;
  alloch_ = h_;

  // It doesn't seem to make any sense to specify only one of the 'format' and
  // 'type' parameters. Until we decide if it makes sense, it's not allowed.
  if      (format_ == GL_NONE && type_ == GL_NONE)
    vgui_accelerate::instance()->vgui_choose_cache_format(&format_, &type_);
  else if (format_ != GL_NONE && type_ != GL_NONE)
    { } // ok
  else
    assert(false);

  // To add a new format, you need to:
  // - create a new pixel type in vgui_pixel.h. Make sure the size of that pixel type
  //   is the same as that of the corresponding GL type.
  // - add the format type to internals/vgui_gl_selection_macros.h. Make sure to
  //   only conditionally include your type unless you are certain that all OpenGL
  //   implementations will support that type.

#define Code( BufferType ) \
      buffer_ = new BufferType [ allocw_*alloch_ ];

  ConditionListBegin;
  ConditionListBody( format_, type_ );
  ConditionListFail {
    vcl_cerr << __FILE__ << ": " << __LINE__ << ": unknown GL format ("
             << format_ << ") and type (" << type_ << ").\n"
             << "You can probably easily add support here.\n";
    assert( false );
  }

#undef Code

  // make sure allocw_ and alloch_ have been initialized.
  assert( allocw_*alloch_ >= w_*h_ );
}

//: Destructor.

vgui_vil2_section_buffer::
~vgui_vil2_section_buffer()
{
#define Code( BufferType ) \
      delete[] (BufferType*)buffer_;

  ConditionListBegin;
  ConditionListBody( format_, type_ );
  ConditionListFail {
    assert( false );
  }

#undef Code
}

//------------------------------------------------------------------------------

//: just draw the outline of the given region.
bool
vgui_vil2_section_buffer::
draw_as_rectangle(float x0, float y0,  float x1, float y1) const
{
  glColor3i(0, 1, 0); // is green good for everyone?
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x0, y0);
  glVertex2f(x1, y0);
  glVertex2f(x1, y1);
  glVertex2f(x0, y1);
  glEnd();
  return true;
}

//: draw the given region using glDrawPixels(), possibly accelerated.
bool
vgui_vil2_section_buffer::
draw_as_image(float x0, float y0,  float x1, float y1) const
{
  if ( !conversion_okay_ ) {
    vgui_macro_warning << "bad conversion in draw_as_image()\n";
    return draw_as_rectangle( x0, y0, x1, y1 );
  }

  return vgui_section_render( buffer_,
                              allocw_, alloch_,
                              x0,y0, x1,y1,
                              format_, type_ );
}
