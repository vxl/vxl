#ifndef vgui_test_sptr_h_
#define vgui_test_sptr_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vgui/vgui_tableau_sptr.h>

class vgui_test_thingy2d;
typedef vgui_tableau_sptr_t<vgui_test_thingy2d> vgui_test_thingy2d_sptr;

class vgui_test_thingy3d;
typedef vgui_tableau_sptr_t<vgui_test_thingy3d> vgui_test_thingy3d_sptr;

class vgui_test_credits;
typedef vgui_tableau_sptr_t<vgui_test_credits> vgui_test_credits_sptr;

#endif // vgui_test_sptr_h_
