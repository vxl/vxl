// This is oxl/xcv/xcv_processing.cxx
#include "xcv_processing.h"
//:
// \file
// See xcv_processing.h for a description of this file.

#include <vil1/vil1_image.h>
#include <vil1/vil1_smooth.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include <xcv/xcv_image_tableau.h>

extern void get_current(unsigned*, unsigned*);
extern bool get_image_at(vil1_image*, unsigned, unsigned);
extern void add_image(vil1_image& img);

// dup image
void xcv_processing::xcv_processing_dup()
{
  unsigned col, row;
  get_current(&col, &row);
  vil1_image img;
  bool image_ok = get_image_at(&img, col, row);
  if (!image_ok)
    return;
  static double sigma = 1.0;

  vgui_dialog dlg("Gaussian smoothing");
  dlg.field("Sigma (pixels)", sigma);
  if (!dlg.ask())
    return;

  img = vil1_smooth_gaussian(img, sigma);
  add_image(img);
}

//-----------------------------------------------------------------------------
//: Creates a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
vgui_menu xcv_processing::create_processing_menu()
{
  vgui_menu pro_menu;
  pro_menu.add("Gaussian smoothing", xcv_processing_dup);

  return pro_menu;
}
