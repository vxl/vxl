#include <vgui/vgui.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_viewer3D.h>
#include <vgui/vgui_test.h>

#include <vrml/vgui_vrml.h>

int main(int argc, char **argv)
{
  if (argc < 3) {
    vcl_cerr << "First cpmmand line argument: image file\n"
             << "Second command line argument: VRML camera file\n";
    return 1;
  }

  vgui::init(argc, argv);

  // an image
  vgui_image_tableau image(argv[1]);
  vgui_viewer2D viewer2D(&image);

  // a vrml model
  vgui_vrml vrml_nice(argv[2]);
  vgui_viewer3D viewer3D(&vrml_nice);

  // credits
  vgui_test::credits text;

  // compose
  vgui_composite comp(&viewer2D, &viewer3D, &text);

#if 0
  vgui_shell_tableau shell(&comp);
  return vgui::run(&shell, 512, 512, "test_composite");
#endif // 0
  return vgui::run(&comp, 512, 512, "test_composite");
}
