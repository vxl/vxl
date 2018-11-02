//:
// \file
// \brief  This example pick a line, box or a point with a pick tableau
// \author Kongbin Kang
// \date   2007-07-09
//

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_range_map.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_clear_tableau.h>

//global pointer to the rubberband tableau
static vgui_easy2D_tableau_sptr easy = nullptr;
static vgui_image_tableau_sptr itab = nullptr;
static vgui_shell_tableau_sptr shell = nullptr;

//the meunu callback functions
static void range_mapping()
{
  static float min_val = 0, max_val = 65535;
  static float gamma = 1.0;
  static bool invert = false;
  static bool use_glPixelMap = true;
  static bool cache_buffer = true;

  vgui_dialog range_dialog("Range Mapping");
  range_dialog.field("Min", min_val);
  range_dialog.field("Max", max_val);
  range_dialog.field("Gamma", gamma);
  range_dialog.checkbox("use GL Pixel Map", use_glPixelMap);
  range_dialog.checkbox("Cache Buffer", cache_buffer);


  if(range_dialog.ask())
  {
    vgui_range_map_params_sptr rmp =
      new vgui_range_map_params(min_val,max_val, gamma,
          invert, use_glPixelMap, cache_buffer);
    //start range mapping
    itab->set_mapping(rmp);
    itab->post_redraw();
  }
}

static void background_color()
{
  static float r = 0, g =0, b = 0, a = 0;

  vgui_dialog dialog("background color");
  dialog.field("Red", r);
  dialog.field("Green", g);
  dialog.field("Blue", b);
  dialog.field("Alpha", a);


  if(dialog.ask())
  {
    shell->get_clear()->set_colour(r, g, b, a);
    shell->post_redraw();
  }
}


// Create the edit menu
vgui_menu create_menus()
{
  vgui_menu view;
  view.add("Range Mapping",range_mapping,(vgui_key)'r',vgui_CTRL);
  view.add("Background", background_color, (vgui_key)'b', vgui_CTRL);
  vgui_menu bar;
  bar.add("View",view);
  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
  {
    std::cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Make the tableau hierarchy.
  itab = bgui_image_tableau_new();
  vil_image_resource_sptr res = vil_load_image_resource(argv[1]);
  itab->set_image_resource(res);

  easy = vgui_easy2D_tableau_new(itab);
  vgui_viewer2D_tableau_new viewer(easy);
  shell = vgui_shell_tableau_new(viewer);

  // Create and run the window
  return vgui::run(shell, 512, 512, create_menus());
}
