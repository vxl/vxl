// This is core/vgui/vgui_blender_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   27 Oct 99
// \brief  See vgui_blender_tableau.h for a description of this file.

#include "vgui_blender_tableau.h"

#include <vcl_iostream.h>

#include <vil1/vil1_load.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_image_renderer.h>
#if 0
# include <vgui/vgui.h>
# define debug vgui::out
#else
# define debug vcl_cerr
#endif

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_blender_tableau_new.
//  Creates a blender with the given image and alpha value.
vgui_blender_tableau::vgui_blender_tableau(char const* file, float a)
  : alpha(a)
{
  renderer = new vgui_image_renderer;
  renderer->set_image(vil1_load(file));
  filename = vcl_string(file);
}

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_blender_tableau_new.
//  Creates a blender with the given image and alpha value.
vgui_blender_tableau::vgui_blender_tableau(vil1_image const& img, float a)
  : alpha(a)
{
  renderer = new vgui_image_renderer;
  renderer->set_image(img);
  filename = vcl_string("unknown");
}

//-----------------------------------------------------------------------------
//: Destructor - called by vgui_blender_tableau_sptr.
vgui_blender_tableau::~vgui_blender_tableau()
{
}

//-----------------------------------------------------------------------------
//: Returns the filename of the loaded image (if it was loaded from file).
vcl_string vgui_blender_tableau::file_name() const
{
  return filename.c_str();
}

//-----------------------------------------------------------------------------
// Returns the type of this tableau ('vgui_blender_tableau').
vcl_string vgui_blender_tableau::type_name() const
{
  return "vgui_blender_tableau";
}

//-----------------------------------------------------------------------------
//: Tell the blender that the image pixels have been changed.
void vgui_blender_tableau::reread_image()
{
  renderer->reread_image();
}

//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
//  In particular, use draw events to draw the blended image.
//  Use '_' and '+' key-press events to change alpha.
bool vgui_blender_tableau::handle(vgui_event const &e)
{
  if (vgui_matrix_state::gl_matrices_are_cleared()) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int width = vp[2];
    int height = vp[3];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0.0, -height, 0.0);
  }

  if (e.type == vgui_DRAW) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelTransferf(GL_ALPHA_SCALE, alpha);

    renderer->render();

    glPixelTransferf(GL_ALPHA_SCALE, 1.0);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);

    return true;
  }

  if (e.type == vgui_KEY_PRESS) {
    switch (e.key)
    {
     case '_':
      alpha -= 0.1f;
      if (alpha <= 0.0f) alpha = 0.0f;
      debug << "blender : alpha = " << alpha << vcl_endl;
      post_redraw();
      return true;
     case '+':
      alpha += 0.1f;
      if (alpha >= 1.0f) alpha = 1.0f;
      debug << "blender : alpha = " << alpha << vcl_endl;
      post_redraw();
      return true;
     default:
      break;
    }
  }
  return false;
}
