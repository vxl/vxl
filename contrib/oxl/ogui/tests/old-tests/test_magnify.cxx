//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_enhance
// Author: Philip C. Pritchett, Oxford RRG
// Created: 17 Nov 99
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_enhance.h>

int main(int argc, char ** argv)
{
  if (argc < 2) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau image1(argv[1]);

  vgui_enhance enhance(&image1);

  vgui_viewer2D viewer(&enhance);

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512, "test_enhance");
#endif // 0
  return vgui::run(&viewer, 512, 512, "test_enhance");
}
