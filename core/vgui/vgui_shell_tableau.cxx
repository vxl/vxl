// \author fsm

#include "vgui_shell_tableau.h"

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_tview_launcher_tableau.h>
#include <vcl_iostream.h>

vgui_event_condition vgui_shell_tableau_bindings::default_quit  = vgui_event_condition( vgui_key('q'), vgui_ALT );
vgui_event_condition vgui_shell_tableau_bindings::default_close = vgui_event_condition( vgui_key('w'), vgui_ALT );
vgui_event_condition vgui_shell_tableau_bindings::default_graph = vgui_event_condition( vgui_key('g'), vgui_SHIFT );


vcl_string vgui_shell_tableau::type_name() const
{
  return "vgui_shell_tableau";
}

vgui_shell_tableau::vgui_shell_tableau()
{
  init();
}

vgui_shell_tableau::vgui_shell_tableau(vgui_tableau_sptr const &t0)
{
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

void vgui_shell_tableau::init()
{
  clear = vgui_clear_tableau_new();
  graph = vgui_tview_launcher_tableau_new();

  vgui_composite_tableau::add(clear);
  vgui_composite_tableau::add(graph);

  set_quit( true );
  set_enable_key_bindings( false );
}

vgui_shell_tableau::~vgui_shell_tableau()
{
  vgui_composite_tableau::remove(graph);
  vgui_composite_tableau::remove(clear);
}

//--------------------------------------------------------------------------------

#include <vgui/vgui_text_graph.h>

bool vgui_shell_tableau::handle(vgui_event const &e)
{
  if ( bindings.quit(e) ) {
    e.origin->post_destroy();
    vgui::quit();
  }

  if ( bindings.close(e) ) {
    e.origin->post_destroy();
  }

  if ( bindings.graph(e) ) {
    vgui_text_graph(vcl_cerr);
  }

  // "draw" event : return true unless some child returns false.
  if ( e.type==vgui_DRAW || e.type==vgui_DRAW_OVERLAY ) {
    bool retv = true;

    for ( unsigned i=0; i<children.size(); ++i ) {
      //PM.restore();
#ifdef DEBUG
      vcl_cerr << "DRAW";
#endif
      if ( active[i] && children[i] )
        if ( !children[i]->handle(e) )
          retv=false;
    }
    return retv;
  }

  // "normal" event : pass it on till handled.
  for ( unsigned i=0; i < children.size(); ++i ) {
    if (active[i] && children[i])
      if ( children[i]->handle(e) )
        return true;
  }

  // Noone was interested....
  return false;
}

void vgui_shell_tableau::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  // add clear tableau
  clear->get_popup(params, menu);

  // add graph launcher
  graph->get_popup(params, menu);

  menu.separator();

  // we skip the first two children.
  for (unsigned i=2; i<children.size(); ++i)
    children[i]->get_popup(params, menu);
}

// mimic the old behaviour. The old behaviour is: the graph condition
// is checked iff enable_key_bindings==true; close and quit conditions
// are checked iff enable_key_bindings==true *and* do_quit==true.
//
void vgui_shell_tableau::set_quit(bool on)
{
  do_quit = on;
  if ( do_quit ) {
    bindings.set_quit( key_bindings_type::default_quit );
    bindings.set_close( key_bindings_type::default_close );
  } else {
    bindings.set_quit( vgui_event_condition() );
    bindings.set_close( vgui_event_condition() );
  }
}

// mimic the old behaviour. See comment above.
void vgui_shell_tableau::set_enable_key_bindings(bool on)
{
  enable_key_bindings = on;
  if ( enable_key_bindings ) {
    bindings.set_graph( key_bindings_type::default_graph );
    // enable quit and close bindings depending on current value of do_quit.
    this->set_quit( do_quit );
  } else {
    // disable all bindings
    bindings.set_quit( vgui_event_condition() );
    bindings.set_close( vgui_event_condition() );
    bindings.set_graph( vgui_event_condition() );
  }
}
