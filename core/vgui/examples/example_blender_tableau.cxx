//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_blend
// Author: Philip C. Pritchett, Oxford RRG
// Created: 26 Oct 99
//
//-----------------------------------------------------------------------------

#include <vgui/vgui.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_blender_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

int main(int argc, char ** argv)
{
  if (argc < 3) return 1;

  vgui::init(argc, argv);

  vgui_blender_tableau_new blend(argv[1], 0.5);
  vgui_image_tableau_new image(argv[2]);

  vgui_composite_tableau_new comp(image, blend);

  vgui_viewer2D_tableau_new viewer(comp);
  return vgui::run(comp, 512, 512, __FILE__);
}
