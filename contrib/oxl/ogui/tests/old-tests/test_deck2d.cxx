#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>

int main(int argc, char **argv)
{
  if (argc < 13) return 1;

  vgui::init(argc, argv);

  vgui_deck_tableau deck;

  for (int i=0; i<12; ++i) {

    vgui_image_tableau *image =
      new vgui_image_tableau( argv[i+1] );

    deck.add(image);
  }

  vgui_viewer2D viewer(&deck);

#if 0
  vgui_shell_tableau shell(&deck);
  return vgui::run(&shell, 512, 512, "test_deck2d");
#endif // 0
  return vgui::run(&deck, 512, 512, "test_deck2d");
}
