//:
// \file
// \author Philip C. Pritchett, Oxford RRG
// \date   25 Jan 00
//-----------------------------------------------------------------------------

#include <vgui/vgui.h>
#include <vgui/vgui_active_visible.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>

int main(int argc, char ** argv)
{
  vgui::init(argc, argv);

  vgui_image_tableau image(argc>1 ? argv[1] : "az32_10.tif");
  vgui_viewer2D viewer2D(&image);
  vgui_active_visible actvis(&viewer2D);

#if 0
  vgui_shell_tableau shell(&actvis);
  return vgui::run(&shell, 512, 512, "test_active_visible");
#endif // 0
  return vgui::run(&actvis, 512, 512, "test_active_visible");
}
