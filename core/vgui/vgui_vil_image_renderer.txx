// This is core/vgui/vgui_vil2_image_renderer.txx
#ifndef vgui_vil2_image_renderer_txx_
#define vgui_vil2_image_renderer_txx_
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil2_image_renderer.h for a description of this file.
//
// Cut-n-paste and modify from vil_image_renderer.cxx

#include "vgui_vil2_image_renderer.h"

#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_vil2_section_buffer.h>
#include <vgui/vgui_vil2_section_buffer_apply.h>
#include <vgui/vgui_projection_inspector.h>

// Only check-in false:
static const bool debug = false;
#define trace if (true) { } else vcl_cerr


//-----------------------------------------------------------------------------
//: Constructor - create an empty image renderer.
template<typename T>
vgui_vil2_image_renderer<T>::vgui_vil2_image_renderer()
  : buffer_(0)
{
}

//-----------------------------------------------------------------------------
//: Destructor - delete image buffer.
template<typename T>
vgui_vil2_image_renderer<T>::~vgui_vil2_image_renderer() 
{
  delete buffer_;
}

//-----------------------------------------------------------------------------
//: Attach the renderer to a new vil_image.
template<typename T>
void
vgui_vil2_image_renderer<T>::set_image( vil2_image_view<T> const &image )
{
  if ( image == the_image_ )
    return; // same image -- do nothing.

  // delete old buffer. we could try to reuse it.
  delete buffer_;
  buffer_ = 0;

  //
  the_image_ = image;
  if ( the_image_ )
    trace << "image : " << the_image_ << vcl_flush;
}

//-----------------------------------------------------------------------------
//: Tell the image renderer that the image has been changed, and should be re-read.
template<typename T>
void
vgui_vil2_image_renderer<T>::reread_image()
{
  delete buffer_;
  buffer_ = 0;
}

//-----------------------------------------------------------------------------
// draw the image :
template<typename T>
void
vgui_vil2_image_renderer<T>::render()
{
  if ( !the_image_ )
    return;

  // Delay sectioning until first render time. This allows the section
  // buffer to decide on a cache format which depends on the current GL
  // rendering context.
  if ( !buffer_ ) {
    buffer_ = new vgui_vil2_section_buffer(0, 0,
                                           the_image_.ni(), the_image_.nj(),
                                           GL_NONE, GL_NONE );
    vgui_vil2_section_buffer_apply( *buffer_, the_image_ );
  }

  buffer_->draw_as_image() || buffer_->draw_as_rectangle();
}

#endif // vgui_vil2_image_renderer_txx_
