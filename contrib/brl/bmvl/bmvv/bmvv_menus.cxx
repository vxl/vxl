#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <bmvv/bmvv_multiview_manager.h>
#include <bmvv/bmvv_menus.h>

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

void bmvv_menus::clear_selected_callback()
{
  bmvv_multiview_manager::instance()->clear_selected();
}

void bmvv_menus::vd_edges_callback()
{
  bmvv_multiview_manager::instance()->vd_edges();
}

void bmvv_menus::track_edges_callback()
{
  bmvv_multiview_manager::instance()->track_edges();
}

#ifdef HAS_XERCES
void bmvv_menus::read_xml_edges_callback()
{
  bmvv_multiview_manager::instance()->read_xml_edges();
}
#endif

void bmvv_menus::show_epipolar_line_callback()
{
  bmvv_multiview_manager::instance()->show_epipolar_line();
}

void bmvv_menus::select_curve_corres_callback()
{
  bmvv_multiview_manager::instance()->select_curve_corres();
}

//bmvv_menus definition
vgui_menu bmvv_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
#ifdef HAS_XERCES
  menufile.add( "Load XML Edges", read_xml_edges_callback);
#endif
  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add("Clear Selected", clear_selected_callback);
  menuview.add( "Epipolar Line", show_epipolar_line_callback,
                (vgui_key)'e', vgui_CTRL);
  //edit menu entries
  menuedit.add("VD Edges", vd_edges_callback);
  menuedit.add("Select Edges", select_curve_corres_callback);
  menuedit.add("Track Edges", track_edges_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

