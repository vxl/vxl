// Example of using a vgui_blender_tableau.

#include <iostream>
#include <vgui/vgui.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_blender_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int main(int argc, char ** argv)
{
  if (argc < 3) {
    std::cerr << "Provide two images as arguments\n";
    return 1;
  }

  vgui::init(argc, argv);

  vgui_blender_tableau_new blend(argv[1], nullptr, 0.5);
  vgui_image_tableau_new image(argv[2]);

  vgui_composite_tableau_new comp(image, blend);

  vgui_viewer2D_tableau_new viewer(comp);
  return vgui::run(viewer, 512, 512, __FILE__);
}
