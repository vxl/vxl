// This is core/vgui/vgui_roi_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Marko Bacic, RRG, University of Oxford
// \date   18 Jul 2000
// \brief  See vgui_roi_tableau.h for a description of this file.

#include "vgui_roi_tableau.h"
#include <vcl_string.h>

#include <vil1/vil1_load.h>
#include <vil1/vil1_crop.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

//------------------------------------------------------------------------------

vgui_roi_tableau::vgui_roi_tableau()
  : vgui_tableau()
{
  cropped_image_ = 0;
}

vgui_roi_tableau::vgui_roi_tableau(vil1_image const &I,char const *name,
                                   float x,float y,float w,float h)
  : vgui_tableau(),name_(name)
{
  cropped_image_ = vil1_crop(I,int(x+0.5),int(y+0.5),int(w+0.5),int(h+0.5));
  roi_.sx = x;
  roi_.sy = y;
  roi_.width = w;
  roi_.height = h;
}

vgui_roi_tableau::~vgui_roi_tableau() {}

vcl_string vgui_roi_tableau::type_name() const
{
  return "vgui_roi_tableau";
}


vcl_string vgui_roi_tableau::file_name() const
{
  return name_;
}

vcl_string vgui_roi_tableau::pretty_name() const
{
  return type_name() + "[" + name_ + "]";
}

//------------------------------------------------------------------------------

vil1_image vgui_roi_tableau::get_image() const
{
  return cropped_image_;
}
#if 0
// this removes the directory part of a filename :
static inline vcl_string __FILE__rem_dir(const char *s)
{
  char const *slash = vcl_strrchr(s,'/');
  return slash ? slash+1 : s;
}
#endif

void vgui_roi_tableau::set_image(vil1_image const &I)
{
  //  // use the name of the image as the name of the tableau :
  //  name_ = __FILE__rem_dir(I.name().c_str());
  cropped_image_ = vil1_crop( I, int(roi_.sx), int(roi_.sy),
                              int(roi_.width), int(roi_.height));
}

// derived :
void vgui_roi_tableau::set_image(char const *f)
{
  set_image( vil1_load(f ? f : "az32_10.tif") );
}

//------------------------------------------------------------------------------

unsigned vgui_roi_tableau::width() const
{
  return cropped_image_.width();
}

unsigned vgui_roi_tableau::height() const
{
  return cropped_image_.height();
}

bool vgui_roi_tableau::get_bounding_box(float low[3], float high[3]) const
{
  low[0] = 0; high[0] = width();
  low[1] = 0; high[1] = height();
  low[2] = 0; high[2] = 0; // why not ?
  return true;
}

//------------------------------------------------------------------------------

bool vgui_roi_tableau::handle(vgui_event const &e)
{
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

  //
  if (e.type == vgui_DRAW) {
    // ROI tableau will have only one child
    get_child(0)->draw();
    // Draw a region of interest
    glBegin(GL_LINE_LOOP);
    glVertex2f(roi_.sx,roi_.sy);
    glVertex2f(roi_.sx+roi_.width,roi_.sy);
    glVertex2f(roi_.sx+roi_.width,roi_.sy+roi_.height);
    glVertex2f(roi_.sx,roi_.sy+roi_.height);
    glEnd();

    return true;
  }

  //
  else
    return get_child(0)->handle(e);
}

//------------------------------------------------------------------------------
