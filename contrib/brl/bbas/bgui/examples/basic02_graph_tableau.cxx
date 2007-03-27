#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

static vgui_text_tableau_sptr text = 0;

//Create a popup text display
static void draw_text()
{
  static vcl_string junk = "s";
  text->clear();
  text->set_colour(1,1,1);
  text->add(100,100,junk);
  text->post_redraw();
  junk += "a";
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("Test Popup");
  popup.inline_tableau(s, 200,200);
  if(!popup.ask())
    return;
}
// Simple menu for launching the popup
vgui_menu my_menu()
{
  vgui_menu edit;
  edit.add("Draw text",draw_text);
  edit.add("Quit", vgui::quit);
  vgui_menu bar;
  bar.add("Edit",edit);
  return bar;
}


int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  text = vgui_text_tableau_new();
  vgui_image_tableau_sptr it = vgui_image_tableau_new();
  vgui::run(it, 512, 512, my_menu());
  return 0;
}
