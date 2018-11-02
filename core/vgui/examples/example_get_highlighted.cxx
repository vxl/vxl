// Example of getting highlighted objects.  vgui_easy2D_tableau highlights
// objects when they are nearest to the mouse (so there is only ever
// one highlighted object).
//
// When this program is run, highlighting a line on the display by passing
// the mouse pointer close to it will print a message saying its start point.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_soview2D.h>

struct my_tab : public vgui_easy2D_tableau
{
  my_tab(vgui_tableau_sptr tab) : vgui_easy2D_tableau(tab){}
 ~my_tab() {}

  // Look for mouse motion events and see if the highlighted object has changed.
  bool handle(vgui_event const &e)
  {
    static vgui_soview* old_so = nullptr;
    if (e.type == vgui_MOTION)
    {
      vgui_soview* my_so = get_highlighted_soview();
      if (my_so)
      {
        if (my_so != old_so && my_so->type_name() == "vgui_soview2D_lineseg")
        {
          vgui_soview2D_lineseg* my_line = (vgui_soview2D_lineseg*)my_so;
          // cout its startpoint
          std::cout << my_line->x0 << "," << my_line->y0 << std::endl;
          old_so = my_so;
        }
      }
    }

    // We aren't interested in any other events so pass them to the base class.
    return vgui_easy2D_tableau::handle(e);
  }
};

typedef vgui_tableau_sptr_t<my_tab> my_tab_sptr;

struct my_tab_new : public my_tab_sptr {
  my_tab_new(vgui_tableau_sptr const& i) :
    my_tab_sptr(new my_tab(i)) { }
};

int main(int argc, char ** argv)
{
  if (! vgui::select(argc, argv))
  {
    if (vgui::exists("gtk"))
      vgui::select("gtk");
    else if (vgui::exists("mfc"))
      vgui::select("mfc");
  }

  vgui::init(argc,argv);
  vgui_clear_tableau_sptr cleartab =
    vgui_clear_tableau_new();
  cleartab->set_colour(1,0,0);
  my_tab_new easy(cleartab);
  vgui_viewer2D_tableau_new viewer(easy);
  vgui_shell_tableau_new shell(viewer);

  // Add some objects to our easy2D so we can see what happens when they
  // are highlighted.
  easy->add_line(10,10,200,200);
  easy->add_line(50,0,0,300);

  return vgui::run(shell, 400, 400);
}
