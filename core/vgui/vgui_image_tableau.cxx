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

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_image_tableau_new.
//  Creates an empty image tableau.
vgui_image_tableau::vgui_image_tableau()
  : vgui_tableau()
  , pixels_centered(true)
  , renderer(new vgui_image_renderer)
{ }

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_image_tableau_new.
//  Creates a tableau displaying the given image.
vgui_image_tableau::vgui_image_tableau(vil_image const &I)
  : vgui_tableau()
  , pixels_centered(true)
  , renderer(new vgui_image_renderer)
{ 
  set_image(I); 
}

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_image_tableau_new.
//  Creates a tableau which loads and displays an image from
//  the given file.
vgui_image_tableau::vgui_image_tableau(char const *f)
  : vgui_tableau()
  , name_(f)
  , pixels_centered(true)
  , renderer(new vgui_image_renderer)
{ 
  set_image(f); 
}

//-----------------------------------------------------------------------------
//: Destructor - called by vgui_image_tableau_sptr.
vgui_image_tableau::~vgui_image_tableau()
{
  delete renderer;
  renderer = 0;
}

//-----------------------------------------------------------------------------
//: Returns the type of this tableau ('vgui_image_tableau').
vcl_string vgui_image_tableau::type_name() const
{
  return "vgui_image_tableau";
}


//-----------------------------------------------------------------------------
//: Returns the filename of the loaded image (if loaded from file).
vcl_string vgui_image_tableau::file_name() const
{
  return name_;
}

//-----------------------------------------------------------------------------
//: Returns a nice version of the name, including details of the image file.
vcl_string vgui_image_tableau::pretty_name() const
{
  return type_name() + "[" + name_ + "]";
}

//-----------------------------------------------------------------------------
//: Return the image being rendered by this tableau.
vil_image vgui_image_tableau::get_image() const
{
  return renderer->get_image();
}

//-----------------------------------------------------------------------------
//: Make the given image, the image rendered by this tableau.
void vgui_image_tableau::set_image(vil_image const &I)
{
  // use the name of the image as the name of the tableau :
  renderer->set_image( I );
}

//-----------------------------------------------------------------------------
//: Make image loaded from the given file, the image rendered by this tableau.
void vgui_image_tableau::set_image(char const *f)
{
  set_image( vil_load(f ? f : "az32_10.tif") );
}

//-----------------------------------------------------------------------------
//: Reread the image from file.
void vgui_image_tableau::reread_image()
{
  renderer->reread_image();
}

//-----------------------------------------------------------------------------
//: Width of the image (0 if none).
unsigned vgui_image_tableau::width() const
{
  return renderer->get_image().width();
}

//-----------------------------------------------------------------------------
//: Height of the image (0 if none).
unsigned vgui_image_tableau::height() const
{
  return renderer->get_image().height();
}

//-----------------------------------------------------------------------------
//: Returns the bounding box of the rendered image.
bool vgui_image_tableau::get_bounding_box(float low[3], float high[3]) const
{
  low[0] = 0; high[0] = width();
  low[1] = 0; high[1] = height();
  low[2] = 0; high[2] = 0;
  return true;
}

//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
//  In particular, use draw events to render the image contained in 
//  this tableau.
bool vgui_image_tableau::handle(vgui_event const &e)
{
  if (e.type == vgui_DRAW) 
  {
    // If blending is turned on, there is a severe performance penalty
    // when rendering an image. So, we turn off blending before calling
    // the renderer. In cases where two images are to be blended, a
    // special tableau should be written, eg. vgui_blender_tableau.
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
  else
    return false;
}
