#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <bmvv/bmvv_tomography_manager.h>
#include <bmvv/bmvv_tomg_menus.h>

//Static munu callback functions

void bmvv_tomg_menus::quit_callback()
{
  bmvv_tomography_manager::instance()->quit();
}


void bmvv_tomg_menus::load_image_callback()
{
  bmvv_tomography_manager::instance()->load_image();
}

void bmvv_tomg_menus::save_sinogram_callback()
{
  bmvv_tomography_manager::instance()->save_sinogram();
}

void bmvv_tomg_menus::clear_display_callback()
{
  bmvv_tomography_manager::instance()->clear_display();
}

void bmvv_tomg_menus::clear_selected_callback()
{
  bmvv_tomography_manager::instance()->clear_selected();
}

void bmvv_tomg_menus::sinogram_callback()
{
  bmvv_tomography_manager::instance()->sinogram();
}

//bmvv_tomg_menus definition
vgui_menu bmvv_tomg_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Save Sinogram", save_sinogram_callback);

  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add("Clear Selected", clear_selected_callback);

  //edit menu entries
  menuedit.add("Gaussian Sinogram", sinogram_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

