#include "basic_menus.h"
#include "basic_manager.h"
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>

//Static munu callback functions

void basic_menus::quit_callback()
{
  basic_manager::instance()->quit();
}


void basic_menus::load_image_callback()
{
  basic_manager::instance()->load_image();
}

//basic_menus definitions
vgui_menu basic_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;

  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);

  //main menu bar
  menubar.add( "File", menufile);
  return menubar;
}

