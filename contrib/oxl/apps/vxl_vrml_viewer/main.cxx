// This is oxl/apps/vxl_vrml_viewer/main.cxx

//:
// \file
// \brief Load a vrml file, overlay 3d and view
// \author awf@robots.ox.ac.uk, Nov 2000

#include <vcl_iostream.h>

// vgui basics:
#include <vgui/vgui.h>
// tableaux:
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vrml/vgui_vrml_tableau.h>
// extras:
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

vgui_deck_tableau_sptr thedeck;
vcl_vector<vgui_vrml_tableau_sptr> thevrmls;
vgui_viewer3D_tableau_sptr theviewer;

vgui_tableau_sptr make_3d_tableau(char const* filename)
{
  vgui_vrml_tableau_new vrml(filename, true);

  // Create a easy3D tableau:
  vgui_easy3D_tableau_new list3D;

  list3D->set_point_radius(1);
  // Add a point at the origin
  list3D->set_foreground(1,1,1);
  list3D->add_point(0,0,0);

  // Add a line in the xaxis:
  list3D->set_foreground(1,1,0);
  list3D->add_line(1,0,0, 4,0,0);
  list3D->add_point(5,0,0);

  // Add a line in the yaxis:
  list3D->set_foreground(0,1,0);
  list3D->add_line(0,1,0, 0,4,0);
  list3D->add_point(0,5,0);

  // Add a line in the zaxis:
  list3D->set_foreground(0,0,1);
  list3D->add_line(0,0,1, 0,0,4);
  list3D->add_point(0,0,5);
  list3D->set_point_radius(1);

  // Composite the easy and vrml

  vgui_text_tableau_new text;
  text->add(-.8f,-.8f, filename);

  vgui_composite_tableau_new c(vrml, text);

  thevrmls.push_back(vrml);

  return c;
}

void load_vrml()
{
  vgui_dialog d("Load vrml");
  vcl_string filename = "find_a_vrml_file.wrl";
  vcl_string re = "*.*";
  d.inline_file("Filename", re, filename);
  if (!d.ask())
    return;
  vgui_tableau_sptr tab3d = make_3d_tableau(filename.c_str());
  thedeck->add(tab3d);
}

bool set_style(int i)
{
  if (i == 0) {
    glPointSize(1);
    return true;
  }

  if (i == 1) {
    glPointSize(2);
    return true;
  }

  vcl_cerr << "unknown style " << i << vcl_endl;
  return false;
}

void next_style()
{
  static int the_style = 0;
  if (!set_style(the_style++))
    the_style = 0;

  for (unsigned int i = 0; i < thevrmls.size(); ++i)
    thevrmls[i]->invalidate_vrml();
}

void toggle_headlight()
{
  theviewer->lighting = !theviewer->lighting;
  theviewer->post_redraw();
}

vgui_menu make_menubar()
{
  // Make submenus
  vgui_menu file;
  file.add("Load vrml", load_vrml, vgui_key_CTRL('A'));

  vgui_menu display;
  display.add("Change style", next_style, vgui_key_CTRL('t'));
  display.add("Toggle headlight", toggle_headlight, vgui_key_CTRL('h'));

  // Make bar
  vgui_menu bar;
  bar.add("File", file);
  bar.add("Display", display);
  return bar;
}


int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  vgui_deck_tableau_new deck;
  thedeck = deck;
  for (int i = 1; i < argc; ++i) {
    vgui_tableau_sptr tab3d = make_3d_tableau(argv[i]);
    thedeck->add(tab3d);
  }

  vgui_viewer3D_tableau_new viewer(deck);
  theviewer = viewer;

  vgui_shell_tableau_new shell(viewer);

  vgui_menu menu = make_menubar();

  next_style();

  int w = 512;
  int h = 512;

  return vgui::run(shell, w, h, menu, "vxl vrml viewer");
}
