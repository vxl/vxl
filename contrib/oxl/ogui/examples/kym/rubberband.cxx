// rubberband.cxx
//
#include <vcl_cassert.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui.h>

static vgui_rubberbander_sptr rubber;

static void line()
{
  rubber->rubberband_line();
}

static void infinite_line()
{
  rubber->rubberband_infinite_line();
}

static void circle()
{
  rubber->rubberband_circle();
}

static void polygon()
{
  rubber->rubberband_polygon();
}

static void linestrip()
{
  rubber->rubberband_linestrip();
}

// Create a vgui.menu:
vgui_menu create_menus()
{
  vgui_menu objs;
  objs.add("line",line);
  objs.add("infinite line", infinite_line);
  objs.add("circle",circle);
  objs.add("polygon", polygon);
  objs.add("linestrip",linestrip);
  vgui_menu bar;
  bar.add("Objects",objs);

  return bar;
}

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  assert(argv[1]);
  vgui_image_tableau_new image(argv[1]);

  vgui_easy2D_new easy(image);
  rubber = vgui_rubberbander_new(new vgui_rubberbander_easy2D_client(easy));
  vgui_composite_new comp(easy, rubber);

  vgui_viewer2D_new zoom(comp);
  vgui_shell_tableau_new shell(zoom);

  return vgui::run(shell, image->width(), image->height(), create_menus());
}
