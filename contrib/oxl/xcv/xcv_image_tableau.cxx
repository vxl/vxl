// This is oxl/xcv/xcv_image_tableau.cxx
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include "xcv_image_tableau.h"
//:
// \file
// \author Marko Bacic (u97mb@robots.ox.ac.uk)
// \brief  See xcv_image_tableau.h for a description of this file.
//
// \verbatim
//  Modifications:
//    05-AUG-2002 K.Y.McGaul - Print mouse position on status bar.
//    06-AUG-2002 K.Y.McGaul - Print RGB value on status bar.
// \endverbatim

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vil1/vil1_crop.h"
#include "vil1/vil1_image.h"
#include "vil1/vil1_rgba.h"

#include "vgui/vgui_event.h"
#include "vgui/vgui_gl.h"
#include "vgui/vgui_projection_inspector.h"

extern void post_to_status_bar(const char*);

//--------------------------------------------------------------------------------

xcv_image_tableau::xcv_image_tableau()
  : defined_(false) { }

xcv_image_tableau::xcv_image_tableau(vil1_image const &I)
  : base(I), defined_(false) { }

xcv_image_tableau::xcv_image_tableau(char const *f)
  : base(f), defined_(false) { }

std::string xcv_image_tableau::type_name() const
{
  return "xcv_image_tableau";
}

//--------------------------------------------------------------------------------

vil1_image xcv_image_tableau::get_image() const
{
  if (!defined_)
    return base::get_image();
  else
    return vil1_crop(base::get_image(),
                     int(roi_.x+0.5),int(roi_.y+0.5),
                     int(roi_.width),int(roi_.height));
}

void xcv_image_tableau::set_roi(float x,float y,float w,float h)
{
  defined_ = true;
  roi_.x = x;
  roi_.y = y;
  roi_.width = w;
  roi_.height = h;
}

void xcv_image_tableau::unset_roi()
{
  defined_ = false;
}

//--------------------------------------------------------------------------------
//: Width of the ROI, or if not defined then the width of the whole image.
unsigned xcv_image_tableau::width() const
{
  if (!defined_)
    return base::width();
  else
    return int(roi_.width);
}

//--------------------------------------------------------------------------------
//: Height of the ROI, or if not defined then the height of the whole image.
unsigned xcv_image_tableau::height() const
{
  if (!defined_)
    return base::height();
  else
    return int(roi_.height);
}

//--------------------------------------------------------------------------------
//: Get the low and high values for the ROI.
bool xcv_image_tableau::get_bounding_box(float low[3], float high[3]) const
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
bool xcv_image_tableau::handle(vgui_event const &e)
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
    post_to_status_bar(" ");
  }
  else if (e.type == vgui_BUTTON_UP)
  {
    button_down = false;
  }
  else if (e.type == vgui_MOTION && !button_down)
  {
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)std::floor(pointx), inty = (int)std::floor(pointy);

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
    std::ostringstream str;
    str << '('<<intx<<','<<inty<<")  R="<<int(pixel.r)<<",G="<<int(pixel.g)<<",B="<<int(pixel.b);
    post_to_status_bar(str.str().c_str());
  }

  return base::handle(e);
}

vgui_roi_tableau_make_roi::vgui_roi_tableau_make_roi(xcv_image_tableau_sptr const& imt)
{
  image_tableau_ = imt;
  done_ = false;
}

void vgui_roi_tableau_make_roi::add_box(float x0,float y0,float x1,float y1)
{
  float sx = x0>x1 ? x1:x0;
  float sy = y0>y1 ? y1:y0;
  float w = std::fabs(x1-x0),
        h = std::fabs(y1-y0);

  image_tableau_->set_roi(sx,sy,w,h);
  done_ = true;
}
//--------------------------------------------------------------------------------
