// This is oxl/vgui/vgui_image_renderer.cxx

//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_image_renderer.h for a description of this file.
//
// \verbatim
//  Modifications
//   15-AUG-2000 Marko Bacic, Oxford RRG -- Now uses new routines for image 
//                                          rendering via textures
//   23-AUG-2000 Marko Bacic, Oxford RRG -- Now uses vgui_cache_wizard
//   08-AUG-2000 Marko Bacic, Oxford RRG -- Minor changes
// \endverbatim

#include "vgui_image_renderer.h"

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_section_buffer.h>
#include <vgui/vgui_projection_inspector.h>

// Only check-in false:
static bool debug = false;
#define trace if (true) { } else vcl_cerr

//------------------------------------------------------------------------------

// defined in vgui.cxx
extern bool vgui_images_are_textures;

vgui_image_renderer::vgui_image_renderer()
  : use_texture_mapping(vgui_images_are_textures)
  , buffer(0)
{
}

vgui_image_renderer::~vgui_image_renderer() {
  if (buffer)
    delete buffer;
}

//------------------------------------------------------------------------------

void vgui_image_renderer::need_resection() const {
  // Not implemented since we use only one section buffer at the moment.
  // Ideally, the set_image() method should be implemented by :
  // {
  //   the_image = image_;
  //   need_resection();
  // }
  // Next time the render() method asks for a section_buffer, the buffer
  // would then be resectioned first.
}

void vgui_image_renderer::set_image(vil_image const &image_) {
  if (image_ == the_image)
    return; // same image -- do nothing.

  // delete old buffer. we could try to reuse it.
  if (buffer)
    delete buffer;
  buffer = 0;

  //
  the_image = image_;
  if (the_image)
    trace << "image : " << the_image << vcl_flush;
}

//: Tell the image renderer that the image has been changed, and should be re-read.
void vgui_image_renderer::reread_image()
{
  delete buffer;
  buffer = 0;
}

//------------------------------------------------------------------------------

// draw the image :
void vgui_image_renderer::render() {
  if (!the_image)
    return;

  // Delay sectioning until first render time. This allows the section
  // buffer to decide on a cache format which depends on the current GL
  // rendering context.
  if (!buffer) {
    buffer = new vgui_section_buffer(0, 0,
                                     the_image.width(), the_image.height(),
                                     GL_NONE, GL_NONE,
                                     use_texture_mapping);
    buffer->apply(the_image);
    if (use_texture_mapping)
      buffer->load_image_as_textures(); // --u97mb
  }

  // Use texture mapping if requested (only). If the image fails to
  // render, render its outline.
  if (use_texture_mapping) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    if (debug)
      vcl_cerr << vp[0] << ", " << vp[1] << ", " << vp[0]+vp[2] << ", " << vp[1]+vp[3] << vcl_endl;

    vgui_projection_inspector pi;

    float x0,y0,x1,y1;
    pi.window_to_image_coordinates(vp[0],vp[1],x0,y1);

    pi.window_to_image_coordinates(vp[0]+vp[2],vp[1]+vp[3],x1,y0);
    if (debug) vcl_cerr << "x0 y0:" << x0 << ", " << y0 << vcl_endl;
    if (debug) vcl_cerr << "x1 y1:" << x1 << ", " << y1 << vcl_endl;
    if (x0<0)
      x0 = 0;
    if (y0<0)
      y0 = 0;
    if (x1>the_image.width())
      x1 = the_image.width();
    if (y1>the_image.height())
      y1= the_image.height();
    if (debug) vcl_cerr << "New x1 y1:" << x1 << ", " << y1 << vcl_endl;
    if (debug) vcl_cerr << "New x0 y0:" << x0 << ", " << y0 << vcl_endl;

    buffer->draw_image_as_cached_textures(x0, y0, vcl_fabs(x1-x0), vcl_fabs(y1-y0)) || buffer->draw_as_rectangle();
    vgui_macro_report_errors;
  }
  else {
    // not texturing.
    buffer->draw_as_image() || buffer->draw_as_rectangle();
  }
}


//------------------------------------------------------------------------------
