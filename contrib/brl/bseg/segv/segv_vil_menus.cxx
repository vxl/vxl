#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <segv/segv_vil_segmentation_manager.h>
#include <segv/segv_vil_menus.h>

//Static munu callback functions

void segv_vil_menus::quit_callback()
{
  segv_vil_segmentation_manager::instance()->quit();
}


void segv_vil_menus::load_image_callback()
{
  segv_vil_segmentation_manager::instance()->load_image();
}

void segv_vil_menus::set_range_params_callback()
{
  segv_vil_segmentation_manager::instance()->set_range_params();
}

void segv_vil_menus::clear_display_callback()
{
  segv_vil_segmentation_manager::instance()->clear_display();
}

void segv_vil_menus::harris_corners_callback()
{
  segv_vil_segmentation_manager::instance()->harris_corners();
}

void segv_vil_menus::vd_edges_callback()
{
  segv_vil_segmentation_manager::instance()->vd_edges();
}

void segv_vil_menus::fit_lines_callback()
{
  segv_vil_segmentation_manager::instance()->fit_lines();
}

void segv_vil_menus::fit_conics_callback()
{
  segv_vil_segmentation_manager::instance()->fit_conics();
}

void segv_vil_menus::regions_callback()
{
  segv_vil_segmentation_manager::instance()->regions();
}

void segv_vil_menus::test_inline_viewer_callback()
{
  segv_vil_segmentation_manager::instance()->test_inline_viewer();
}
void segv_vil_menus::test_ellipse_draw_callback()
{
  segv_vil_segmentation_manager::instance()->test_ellipse_draw();
}


//segv_vil_menus definition
vgui_menu segv_vil_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);

  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add("Set Range Params", set_range_params_callback);
  menuview.add("Test Inline Viewer", test_inline_viewer_callback);
  menuview.add("Test Draw Conic", test_ellipse_draw_callback);

  //edit menu entries
  menuedit.add("Harris", harris_corners_callback);
  menuedit.add("VD Edges", vd_edges_callback);
  menuedit.add("Line Segments", fit_lines_callback);
  menuedit.add("Conic Segments", fit_conics_callback);
  menuedit.add("Edgel Regions", regions_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

