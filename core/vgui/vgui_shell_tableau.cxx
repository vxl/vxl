/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_shell_tableau.h"

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_clear_tableau.h>
//#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_tview_launcher_tableau.h>

vcl_string vgui_shell_tableau::type_name() const {
  return "vgui_shell_tableau";
}

vgui_shell_tableau::vgui_shell_tableau() {
  init();
}

vgui_shell_tableau::vgui_shell_tableau(vgui_tableau_sptr const &t0) {
  init();
  vgui_composite_tableau::add(t0);
}

vgui_shell_tableau::vgui_shell_tableau(vgui_tableau_sptr const &t0,
                                       vgui_tableau_sptr const &t1)
{
  init();
  vgui_composite_tableau::add(t0);
  vgui_composite_tableau::add(t1);
}

vgui_shell_tableau::vgui_shell_tableau(vgui_tableau_sptr const &t0,
                                       vgui_tableau_sptr const &t1,
                                       vgui_tableau_sptr const &t2)
{
  init();
  vgui_composite_tableau::add(t0);
  vgui_composite_tableau::add(t1);
  vgui_composite_tableau::add(t2);
}

vgui_shell_tableau::vgui_shell_tableau(vgui_tableau_sptr const &t0,
                                       vgui_tableau_sptr const &t1,
                                       vgui_tableau_sptr const &t2,
                                       vgui_tableau_sptr const &t3)
{
  init();
  vgui_composite_tableau::add(t0);
  vgui_composite_tableau::add(t1);
  vgui_composite_tableau::add(t2);
  vgui_composite_tableau::add(t3);
}

void vgui_shell_tableau::init() {
  clear = vgui_clear_tableau_new();
  graph = vgui_tview_launcher_tableau_new();

  vgui_composite_tableau::add(clear);
  vgui_composite_tableau::add(graph);

  do_quit = true;
  enable_key_bindings = false;
}

vgui_shell_tableau::~vgui_shell_tableau() {
  vgui_composite_tableau::remove(graph);
  vgui_composite_tableau::remove(clear);
}

//--------------------------------------------------------------------------------

#include <vgui/vgui_text_graph.h>

bool vgui_shell_tableau::handle(vgui_event const &e) {

  if (enable_key_bindings && e.type == vgui_KEY_PRESS) {
    if (do_quit && ((e.key=='q' && (e.modifier & vgui_META || e.modifier & vgui_ALT))  || e.key==vgui_ESCAPE)) {
      e.origin->post_destroy();
      vgui::quit();
    }
    if (do_quit && (e.key=='w' && (e.modifier & vgui_META || e.modifier & vgui_ALT)))
      e.origin->post_destroy();

    if (e.key == 'G')
      vgui_text_graph(vcl_cerr);
  }

  // save current matrix state :
  //vgui_matrix_state PM;

  // "draw" event : return true unless some child returns false.
  if (e.type==vgui_DRAW || e.type==vgui_DRAW_OVERLAY) {
    bool retv = true;

    for (unsigned i=0; i<children.size(); ++i) {
      //PM.restore();
      //vcl_cerr << "DRAW";
      if (active[i] && children[i])
        if ( !children[i]->handle(e) )
          retv=false;
    }
    return retv;
  }

  // "normal" event : pass it on till handled.
  for (unsigned i=0; i<children.size(); ++i) {
    //PM.restore();
    //vcl_cerr << "EVEM";
    if (active[i] && children[i])
      if ( children[i]->handle(e) )
        return true;
  }

  // Noone was interested....
  return false;
}

void vgui_shell_tableau::get_popup(vgui_popup_params const &params, vgui_menu &menu) {
  // add clear tableau
  clear->get_popup(params, menu);

  // add graph launcher
  graph->get_popup(params, menu);

  menu.separator();

  // we skip the first two children.
  for (unsigned i=2; i<children.size(); ++i)
    children[i]->get_popup(params, menu);
}
