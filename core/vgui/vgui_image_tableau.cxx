// This is oxl/vgui/vgui_image_tableau.cxx
#include "vgui_image_tableau.h"
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_image_tableau.h for a description of this file.
//
// \verbatim
//  Modifications:
//    15-AUG-2000 Marko Bacic,Oxford RRG -- Removed legacy ROI
// \endverbatim


#include <vcl_string.h>

#include <vil/vil_load.h>

#include <vgui/vgui_image_renderer.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

//--------------------------------------------------------------------------------

vgui_image_tableau::vgui_image_tableau()
  : vgui_tableau()
  , pixels_centered(true)
  , renderer(new vgui_image_renderer)
{ }

vgui_image_tableau::vgui_image_tableau(vil_image const &I)
  : vgui_tableau()
  , pixels_centered(true)
  , renderer(new vgui_image_renderer)
{ set_image(I); }

vgui_image_tableau::vgui_image_tableau(char const *f)
  : vgui_tableau()
  , name_(f)
  , pixels_centered(true)
  , renderer(new vgui_image_renderer)
{ set_image(f); }


vgui_image_tableau::~vgui_image_tableau()
{
  delete renderer;
  renderer = 0;
}

vcl_string vgui_image_tableau::type_name() const
{
  return "vgui_image_tableau";
}


vcl_string vgui_image_tableau::file_name() const
{
  return name_;
}

vcl_string vgui_image_tableau::pretty_name() const
{
  return type_name() + "[" + name_ + "]";
}

//--------------------------------------------------------------------------------

vil_image vgui_image_tableau::get_image() const
{
   return renderer->get_image();
}

void vgui_image_tableau::set_image(vil_image const &I)
{
  //  // use the name of the image as the name of the tableau :
  renderer->set_image( I );
}

// derived :
void vgui_image_tableau::set_image(char const *f)
{
  set_image( vil_load(f ? f : "az32_10.tif") );
}

void vgui_image_tableau::reread_image()
{
  renderer->reread_image();
}

//--------------------------------------------------------------------------------

unsigned vgui_image_tableau::width() const
{
    return renderer->get_image().width();
}

unsigned vgui_image_tableau::height() const
{
    return renderer->get_image().height();
}

bool vgui_image_tableau::get_bounding_box(float low[3], float high[3]) const
{
  low[0] = 0; high[0] = width();
  low[1] = 0; high[1] = height();
  low[2] = 0; high[2] = 0;
  return true;
}

//--------------------------------------------------------------------------------

bool vgui_image_tableau::handle(vgui_event const &e)
{
  // If we allow this, programs which depend on it will fail as
  // soon as the image is put in another tableau.
#if 0
  // if GL matrices are zero, set them to something sensible :
  if (vgui_matrix_state::gl_matrices_are_cleared()) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int width = vp[2];
    int height = vp[3];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
#endif

  //
  if (e.type == vgui_DRAW) {
    // If blending is turned on, there is a severe performance penalty
    // when rendering an image. So, we turn off blending before calling
    // the renderer. In cases where two images are to be blended, a
    // special tableau should be written, eg. vgui_image_blender.
    // fsm@robots.ox.ac.uk
    GLboolean blend_on;
    glGetBooleanv(GL_BLEND, &blend_on);
    if (blend_on)
      glDisable(GL_BLEND);

    if (pixels_centered)
      glTranslated(-0.5, -0.5, 0);

    renderer->render();

    if (pixels_centered)
      glTranslated(+0.5, +0.5, 0);

    if (blend_on)
      glEnable(GL_BLEND);

    return true;
  }

  //
  else
    return false;
}

//--------------------------------------------------------------------------------
