#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_menu.h>
#include <bmvv/bmvv_multiview_manager.h>
#include "bmvv_menus.h"

//Static munu callback functions

void bmvv_menus::quit_callback()
{
  bmvv_multiview_manager::instance()->quit();
}


void bmvv_menus::load_image_callback()
{
  bmvv_multiview_manager::instance()->load_image();
}

void bmvv_menus::clear_display_callback()
{
  bmvv_multiview_manager::instance()->clear_display();
}

void bmvv_menus::vd_edges_callback()
{
  bmvv_multiview_manager::instance()->vd_edges();
}


void bmvv_menus::read_xml_edges_callback()
{
  bmvv_multiview_manager::instance()->read_xml_edges();
}

void bmvv_menus::show_epipolar_line_callback()
{
  bmvv_multiview_manager::instance()->show_epipolar_line();
}

//bmvv_menus definition
vgui_menu bmvv_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Load XML Edges", read_xml_edges_callback);
  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add( "Epipolar Line", show_epipolar_line_callback,
                (vgui_key)'e', vgui_CTRL);
  //edit menu entries
  menuedit.add("VD Edges", vd_edges_callback);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

