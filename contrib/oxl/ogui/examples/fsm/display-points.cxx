/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>

struct example_tab : vgui_tableau
{
  vcl_vector<vcl_pair<float, float> > points;

  bool handle(vgui_event const &e) {
    if (e.type == vgui_DRAW) {
      glPointSize(3);
      glColor3f(1, 0, 0); // red
      //glColor3f(0, 1, 0); // green
      //glColor3f(0, 0, 1); // blue
      glBegin(GL_POINTS);
      for (unsigned i=0; i<points.size(); ++i)
        // +0.5 to put the point in the middle of the relevant pixel.
        // FIXME
        glVertex2f(points[i].first  + 0.5,
                   points[i].second + 0.5);
      glEnd();
      return true;
    }
    else
      return false;
  }
};
struct example_tab_new : vgui_tableau_sptr_t<example_tab> {
  example_tab_new() : vgui_tableau_sptr_t<example_tab>(new example_tab) { }
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  
  vgui_image_tableau_new image_tab(argv[1] ? argv[1] : "pig:0");
  
  example_tab_new example_tab_tab;

  vcl_cerr << "reading points from stdin..." << vcl_flush;
  while (vcl_cin && !vcl_cin.eof() && vcl_cin.good()) {
    float x = -1, y = -1;
    vcl_cin >> x >> y;
    if (vcl_cin.good())
      example_tab_tab->points.push_back(vcl_make_pair(x, y));
  }
  vcl_cerr << example_tab_tab->points.size() << " points" << vcl_endl;

  vgui_composite_new comp_tab(image_tab, example_tab_tab);

  vgui_viewer2D_new zoom_tab(comp_tab);
  
  vgui_shell_tableau_new shell_tab(zoom_tab);
  shell_tab->set_enable_key_bindings(true);
  
  return vgui::run(shell_tab, image_tab->width(), image_tab->height());
}
