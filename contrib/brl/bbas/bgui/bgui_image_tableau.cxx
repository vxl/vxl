// This is brl/bbas/bgui/bgui_image_tableau.cxx
#include "bgui_image_tableau.h"
//:
// \file
// \author Marko Bacic (u97mb@robots.ox.ac.uk)
// \brief  See bgui_image_tableau.h for a description of this file.

#include <vcl_cmath.h>
#include <vcl_cstdio.h> // sprintf

#include <vil1/vil1_crop.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_rgba.h>
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_projection_inspector.h>


//--------------------------------------------------------------------------------

bgui_image_tableau::bgui_image_tableau()
  : defined_(false) { }

bgui_image_tableau::bgui_image_tableau(vil1_image const &I)
  : base(I), defined_(false) { }

bgui_image_tableau::bgui_image_tableau(char const *f)
  : base(f), defined_(false) { }

//--------------------------------------------------------------------------------

vil1_image bgui_image_tableau::get_image() const
{
  if (!defined_)
    return base::get_image();
  else
    return vil1_crop(base::get_image(),
                     int(roi_.x+0.5),int(roi_.y+0.5),
                     int(roi_.width),int(roi_.height));
}

void bgui_image_tableau::set_roi(float x,float y,float w,float h)
{
  defined_ = true;
  roi_.x = x;
  roi_.y = y;
  roi_.width = w;
  roi_.height = h;
}

void bgui_image_tableau::unset_roi()
{
  defined_ = false;
}

//--------------------------------------------------------------------------------
//: Width of the ROI, or if not defined then the width of the whole image.
unsigned bgui_image_tableau::width() const
{
  if (!defined_)
    return base::width();
  else
    return int(roi_.width);
}

//--------------------------------------------------------------------------------
//: Height of the ROI, or if not defined then the height of the whole image.
unsigned bgui_image_tableau::height() const
{
  if (!defined_)
    return base::height();
  else
    return int(roi_.height);
}

//--------------------------------------------------------------------------------
//: Get the low and high values for the ROI.
bool bgui_image_tableau::get_bounding_box(float low[3], float high[3]) const
{
  if (defined_) {
    low[0] = roi_.x;
    low[1] = roi_.y;
    low[2] = 0;
    high[0] = roi_.x+roi_.width;
    high[1] = roi_.y+roi_.height;
    high[2] = 0;
    return true;
  }
  else
    return base::get_bounding_box(low, high);
}

//--------------------------------------------------------------------------------
//: Handle all events for this tableau.
bool bgui_image_tableau::handle(vgui_event const &e)
{
  static bool button_down = false;
  //
  if (e.type == vgui_DRAW) {
    base::handle(e);
    if (defined_) {
      // Draw a region of interest
      glLineWidth(1);
      glColor3f(0,1,0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(roi_.x,roi_.y);
      glVertex2f(roi_.x+roi_.width,roi_.y);
      glVertex2f(roi_.x+roi_.width,roi_.y+roi_.height);
      glVertex2f(roi_.x,roi_.y+roi_.height);
      glEnd();
    }
    return true;
  }
  else if (e.type == vgui_BUTTON_DOWN)
  {
    button_down = true;
    vgui::out << ' ' << vcl_endl;
  }
  else if (e.type == vgui_BUTTON_UP)
  {
    button_down = false;
  }
  else if (e.type == vgui_MOTION && button_down == false)
  {
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);

    // Get RGB value to display on status bar:

    // It's easier to get the buffer in vil1_rgba format and then convert to
    // RGB, because that avoids alignment problems with glReadPixels.
    vil1_rgba<GLubyte> pixel;
    //
    glPixelZoom(1,1);
    glPixelTransferi(GL_MAP_COLOR,0);
    glPixelTransferi(GL_RED_SCALE,1);   glPixelTransferi(GL_RED_BIAS,0);
    glPixelTransferi(GL_GREEN_SCALE,1); glPixelTransferi(GL_GREEN_BIAS,0);
    glPixelTransferi(GL_BLUE_SCALE,1);  glPixelTransferi(GL_BLUE_BIAS,0);

    glPixelStorei(GL_PACK_ALIGNMENT,1);   // byte alignment.
    glPixelStorei(GL_PACK_ROW_LENGTH,0);  // use default value (the arg to pixel routine).
    glPixelStorei(GL_PACK_SKIP_PIXELS,0); //
    glPixelStorei(GL_PACK_SKIP_ROWS,0);   //

    glReadPixels(e.wx, e.wy,             //
                 1, 1,             // height and width (only one pixel)
                 GL_RGBA,          // format
                 GL_UNSIGNED_BYTE, // type
                 &pixel);

    // Display on status bar:
    char msg[100];
    vcl_sprintf(msg, "(%d, %d)   R=%d,G=%d,B=%d", intx, inty,
                (int)pixel.r, (int)pixel.g, (int)pixel.b);
    vgui::out << msg << vcl_endl;
  }

  return base::handle(e);
}

//--------------------------------------------------------------------------------
