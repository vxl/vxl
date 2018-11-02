// \author  fsm
// \brief   Example using vgui_satellite_tableau.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_satellite_tableau.h>
#include <vgui/vgui_satellite_tableau.hxx>

struct example_object
{
  vgui_satellite_tableau_new<example_object> a;
  bool handle_a(vgui_event const &e) {
    std::cerr << "handle_a(): " << e << std::endl;
    return false;
  }

  vgui_satellite_tableau_t_new<example_object, int> b1, b2;
  bool handle_b(vgui_event const &, int d) {
    std::cerr << "handle_b(): " << d << std::endl;
    return false;
  }

  std::string type_name() const { return "example_object"; }

  example_object()
    : a (this, &example_object::handle_a)
    , b1(this, &example_object::handle_b, 1)
    , b2(this, &example_object::handle_b, 2) { }
};

VGUI_SATELLITE_INSTANTIATE(example_object);
VGUI_SATELLITE_T_INSTANTIATE(example_object, int);

#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  example_object sp;
  vgui_deck_tableau_new deck(sp.a, sp.b1, sp.b2);
  return vgui::run(deck, 512, 512);
}
