#ifndef vgui_test_ref_h_
#define vgui_test_ref_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vgui/vgui_tableau_ref.h>

class vgui_test_thingy2d;
typedef vgui_tableau_ref_t<vgui_test_thingy2d> vgui_test_thingy2d_ref;

class vgui_test_thingy3d;
typedef vgui_tableau_ref_t<vgui_test_thingy3d> vgui_test_thingy3d_ref;

class vgui_test_credits;
typedef vgui_tableau_ref_t<vgui_test_credits> vgui_test_credits_ref;

#endif // vgui_test_ref_h_
