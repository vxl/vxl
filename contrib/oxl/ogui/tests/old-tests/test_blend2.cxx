//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_blend
// Author: Philip C. Pritchett, Oxford RRG
// Created: 26 Oct 99
//
//-----------------------------------------------------------------------------

#include <vgui/vgui.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_image_blender.h>
#include <vgui/vgui_viewer2D.h>

int main(int argc, char ** argv)
{
  if (argc < 3) return 1;

  vgui::init(argc, argv);

  vgui_image_blender blend(argv[1]);
  vgui_image_tableau image(argv[2]);

  vgui_viewer2D viewer(&blend);

#if 0
  vgui_shell_tableau tab(&image, &viewer);
#endif // 0
  vgui_composite tab(&image, &viewer);
  return vgui::run(&tab, 512, 512, "test_blend2");
}
