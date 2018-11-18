#include "segv_misc_menus.h"
//:
// \file
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include "segv_misc_manager.h"

// Static menu callback functions

void segv_misc_menus::quit_callback()
{
  segv_misc_manager::instance()->quit();
}


void segv_misc_menus::load_image_callback()
{
  segv_misc_manager::instance()->load_image();
}

void segv_misc_menus::save_image_callback()
{
  segv_misc_manager::instance()->save_image();
}


void segv_misc_menus::project_on_subm_callback()
{
  segv_misc_manager::instance()->project_on_subm();
}

void segv_misc_menus::project_on_articulation_callback()
{
  segv_misc_manager::instance()->project_on_articulation();
}

//segv_misc_menus definition
vgui_menu segv_misc_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menuops;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Save Image", save_image_callback);

  //view menu entries
  menuview.add("Project On Rotation/Scale  Submanifold", project_on_subm_callback);
  menuview.add("Project On Articulation Submanifold", project_on_articulation_callback);

  //edit menu entries

  //operation menu entries

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Image Ops", menuops);
  return menubar;
}
