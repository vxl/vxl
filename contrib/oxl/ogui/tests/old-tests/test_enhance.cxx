//:
// \file
// \author Philip C. Pritchett, Oxford RRG
// \date   17 Nov 99
//-----------------------------------------------------------------------------
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_enhance.h>

int main(int argc, char ** argv)
{
  if (argc < 3) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau image1(argv[1]);
  vgui_image_tableau image2(argv[2]);

  vgui_enhance enhance(&image1, &image2);

  vgui_viewer2D viewer(&enhance);

  return vgui::run(&viewer, 512, 512, "test_enhance");
}
