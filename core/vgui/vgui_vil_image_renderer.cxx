// This is core/vgui/vgui_vil_image_renderer.cxx
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil_image_renderer.h for a description of this file.
//
// Cut-n-paste and modify from vil1_image_renderer.cxx

#include "vgui_vil_image_renderer.h"

#include <vcl_iostream.h>

#include <vil/vil_image_resource.h>

#include "vgui_gl.h"
#include "vgui_macro.h"
#include "vgui_section_buffer.h"

// Only check-in false:
static const bool debug = false;
#define trace if (true) { } else vcl_cerr


vgui_vil_image_renderer::
vgui_vil_image_renderer()
  : buffer_( 0 )
{
}


vgui_vil_image_renderer::
~vgui_vil_image_renderer() 
{
  delete buffer_;
}

void
vgui_vil_image_renderer::
set_image_resource( vil_image_resource_sptr const& image )
{
  // delete old buffer. we could try to reuse it.
  delete buffer_;
  buffer_ = 0;

  the_image_ = image;
  if ( the_image_ )
    trace << "image : " << the_image_ << vcl_flush;
}


vil_image_resource_sptr
vgui_vil_image_renderer::
get_image_resource() const
{
  return the_image_;
}


void
vgui_vil_image_renderer::
reread_image()
{
  delete buffer_;
  buffer_ = 0;
}


void
vgui_vil_image_renderer::
render()
{
  if ( !the_image_ )
    return;

  // Delay sectioning until first render time. This allows the section
  // buffer to decide on a cache format which depends on the current GL
  // rendering context.
  if ( !buffer_ ) {
    buffer_ = new vgui_section_buffer( 0, 0,
                                       the_image_->ni(), the_image_->nj(),
                                       GL_NONE, GL_NONE );
    buffer_->apply( the_image_ );
  }

  buffer_->draw_as_image() || buffer_->draw_as_rectangle();
}
