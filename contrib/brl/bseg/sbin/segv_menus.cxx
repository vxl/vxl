#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_menu.h>
#include <bseg/segv/segv_segmentation_manager.h>
#include <bseg/sbin/segv_menus.h>

//Static munu callback functions

void segv_menus::quit_callback()
{
  segv_segmentation_manager::instance()->quit();
}


void segv_menus::load_image_callback()
{
  segv_segmentation_manager::instance()->load_image();
}

void segv_menus::clear_display_callback()
{
  segv_segmentation_manager::instance()->clear_display();
}

void segv_menus::vd_edges_callback()
{
  segv_segmentation_manager::instance()->vd_edges();
}

void segv_menus::regions_callback()
{
  segv_segmentation_manager::instance()->regions();
}

#ifdef HAS_XERCES
void segv_menus::read_xml_edges_callback()
{
  segv_segmentation_manager::instance()->read_xml_edges();
}
#endif

//segv_menus definition
vgui_menu segv_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
#ifdef HAS_XERCES
  menufile.add( "Load XML Edges", read_xml_edges_callback);
#endif
  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  //edit menu entries
  menuedit.add("VD Edges", vd_edges_callback);
  menuedit.add("Regions", regions_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

