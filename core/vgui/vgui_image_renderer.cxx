// This is core/vgui/vgui_image_renderer.cxx
#include "vgui_image_renderer.h"
//:
// \file
// \author fsm
// \brief  See vgui_image_renderer.h for a description of this file.
//
// \verbatim
//  Modifications
//   15-AUG-2000 Marko Bacic, Oxford RRG -- Now uses new routines for image
//                                          rendering via textures
//   23-AUG-2000 Marko Bacic, Oxford RRG -- Now uses vgui_cache_wizard
//   08-AUG-2000 Marko Bacic, Oxford RRG -- Minor changes
//   06-AUG-2003 Amitha Perera -- Remove texture mapping.
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/vgui_section_buffer.h>

//-----------------------------------------------------------------------------
//: Constructor - create an empty image renderer.
vgui_image_renderer::vgui_image_renderer()
  : buffer(0)
  , use_texture_mapping(false)
{
}

//-----------------------------------------------------------------------------
//: Destructor - delete image buffer.
vgui_image_renderer::~vgui_image_renderer()
{
  if (buffer)
    delete buffer;
}

//-----------------------------------------------------------------------------
void vgui_image_renderer::need_resection() const
{
  // Not implemented since we use only one section buffer at the moment.
  // Ideally, the set_image() method should be implemented by :
  // {
  //   the_image = image_;
  //   need_resection();
  // }
  // Next time the render() method asks for a section_buffer, the buffer
  // would then be resectioned first.
}

//-----------------------------------------------------------------------------
//: Attach the renderer to a new vil1_image.
void vgui_image_renderer::set_image(vil1_image const &image_)
{
  if (image_ == the_image)
    return; // same image -- do nothing.

  // delete old buffer. we could try to reuse it.
  if (buffer)
    delete buffer;
  buffer = 0;

  the_image = image_;
}

//-----------------------------------------------------------------------------
//: Tell the image renderer that the image has been changed, and should be re-read.
void vgui_image_renderer::reread_image()
{
  delete buffer;
  buffer = 0;
}

//-----------------------------------------------------------------------------
// draw the image :
void vgui_image_renderer::render()
{
  if (!the_image)
    return;

  // Delay sectioning until first render time. This allows the section
  // buffer to decide on a cache format which depends on the current GL
  // rendering context.
  if (!buffer) {
    buffer = new vgui_section_buffer(0, 0,
                                     the_image.width(), the_image.height(),
                                     GL_NONE, GL_NONE);
    buffer->apply(the_image);
  }

  buffer->draw_as_image() || buffer->draw_as_rectangle();
}
