// This example displays text in a window.
// Note1, a vgui_text_tableau can have another tableau as a child so you
// could display text on top of an image, etc if you wished.
// Note2, this tableau will not display text unless CMake can find GLUT
// on your system.

#include <vgui/vgui.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  // Make a text tableau and add some text to it.
  vgui_text_tableau_sptr text = vgui_text_tableau_new();
  text->add(100, 100, "Hello world");

  vgui_viewer2D_tableau_new viewer(text);
  vgui_shell_tableau_new shell(viewer);
  return vgui::run(shell, 512, 512);
}
