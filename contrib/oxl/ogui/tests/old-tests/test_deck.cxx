#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_viewer3D.h>
#include <vrml/vgui_vrml.h>

int main(int argc, char **argv)
{
  if (argc < 9) {
    vcl_cerr << "First 6 command line arguments: image files\n"
             << "Next two command line arguments: VRML camera files\n";
    return 1;
  }

  vgui::init(argc, argv);

  vgui_deck_tableau deck;

  for (int i=0; i<6; ++i) {
    vgui_image_tableau *image = new vgui_image_tableau(argv[i+1]);
    vgui_viewer2D *viewer2D = new vgui_viewer2D(image);

    deck.add(viewer2D);
  }

  vgui_vrml vrml_simple(argv[7]);
  vgui_vrml vrml_nice(argv[8]);

  vgui_viewer3D viewer1(&vrml_simple);
  vgui_viewer3D viewer2(&vrml_nice);

  deck.add(&viewer1);
  deck.add(&viewer2);

  deck.notify();

#if 0
  vgui_shell_tableau shell(&deck);
  return vgui::run(&shell, 512, 512, "test_deck");
#endif // 0
  return vgui::run(&deck, 512, 512, "test_deck");
}
