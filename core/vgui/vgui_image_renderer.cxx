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
//   27-DEC-2004 J.L Mundy     -- Added range mapping to control image display
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/vgui_section_buffer.h>
#include <vgui/vgui_range_map_params.h>

//-----------------------------------------------------------------------------
//: Constructor - create an empty image renderer.
vgui_image_renderer::vgui_image_renderer()
  : buffer(0), buffer_params(0), valid_buffer(false), use_texture_mapping(false)
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

void vgui_image_renderer::
create_buffer(vgui_range_map_params_sptr const& rmp)
{
  delete buffer;

  buffer = new vgui_section_buffer(0, 0,
                                     the_image.width(), the_image.height(),
                                     GL_NONE, GL_NONE);
  buffer->apply( the_image, rmp );

  buffer_params = rmp;
  valid_buffer = true;
}

void vgui_image_renderer::
draw_pixels()
{
  buffer->draw_as_image() || buffer->draw_as_rectangle();
}

bool vgui_image_renderer::
render_directly(vgui_range_map_params_sptr const& rmp)
{
  //hardware mapping not currently supported for vil1 images
  if(rmp)//test needed to eliminate warning of unused parameter
    vcl_cout << "No hardware mapping support for vil1 images\n";
  return false;
}

//-----------------------------------------------------------------------------
// draw the image :
void vgui_image_renderer::render(vgui_range_map_params_sptr const& rmp)
{
  if (!the_image)
    return;

  //If the image can be mapped then there is no point in having a
  //GL buffer.  The image can be directly rendered by the hardware
  //using the range map.
  if (rmp&&rmp->use_glPixelMap_&&this->render_directly(rmp))
    return;

  // Delay sectioning until first render time. This allows the section
  // buffer to decide on a cache format which depends on the current GL
  // rendering context.

  if (!this->old_range_map_params(rmp)||!valid_buffer)
    this->create_buffer(rmp);

  this->draw_pixels();
}

//: Are the range map params associated with the current buffer out of date?
//  If so we have to refresh the buffer.
bool vgui_image_renderer::
old_range_map_params(vgui_range_map_params_sptr const& rmp)
{
  //Cases
  
  //1) Both the current params and the new params are null
  if (!buffer_params&&!rmp)
    return true;

  //2) The current params are null and the new params are not
  if (!buffer_params&&rmp)
    return false;

  //3) The current params are not null and the new params are
  if (buffer_params&&!rmp)
    return false;

  //4) Both current params and the new params are not null.
  // Are they equal?
  if (buffer_params&&rmp)
    return *buffer_params==*rmp;

  return false;
}
