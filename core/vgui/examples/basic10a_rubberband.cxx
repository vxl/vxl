// This example creates a line or a circle with a rubberbanding event loop

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//global pointer to the rubberband tableau
static vgui_rubberband_tableau_sptr rubber = 0;

//the meunu callback functions
static void create_line()
{
  rubber->rubberband_line();
}

static void create_circle()
{
  rubber->rubberband_circle();
}

// Create the edit menu
vgui_menu create_menus()
{
  vgui_menu edit;
  edit.add("CreateLine",create_line,(vgui_key)'l',vgui_CTRL);
  edit.add("CreateCircle",create_circle,(vgui_key)'k',vgui_CTRL);
  vgui_menu bar;
  bar.add("Edit",edit);
  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Make the tableau hierarchy.
  vgui_image_tableau_new image(argv[1]);
  vgui_easy2D_tableau_new easy(image);
  vgui_rubberband_easy2D_client* r_client =
    new vgui_rubberband_easy2D_client(easy);
  rubber = vgui_rubberband_tableau_new(r_client);
  vgui_composite_tableau_new comp(easy, rubber);
  vgui_viewer2D_tableau_new viewer(comp);
  vgui_shell_tableau_new shell(viewer);

  // Create and run the window
  return vgui::run(shell, 512, 512, create_menus());
}
