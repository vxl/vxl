//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_image
// Author: Philip C. Pritchett, Oxford RRG
// Created: 05 Oct 99
//
//-----------------------------------------------------------------------------

#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_zoomer.h>
#include <vgui/vgui_viewer2D.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv); // must come before argparse.

  char const *file = argv[1] ? argv[1] : "az32_10.tif";
  vil1_image I = vil1_load(file);
  vcl_cerr << "I = " << I << vcl_endl;

  vgui_image_tableau image(I);

#if 1
  vgui_zoomer zr(&image);
  vgui_load viewer(&zr);
  viewer.set_image(I.width(), I.height());
#else
  vgui_viewer2D viewer(&image);
#endif

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, I.width(), I.height(), file);
#endif // 0
  return vgui::run(&viewer, I.width(), I.height(), file);
}
