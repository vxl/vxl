/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_toolkit.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

#include <vbl/vbl_trace.h>

#include <vgui/vgui_macro.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_adaptor.h>

vcl_vector<vgui_toolkit*> *vgui_toolkit::registry()
{
  static vcl_vector<vgui_toolkit*> *the_vector = 0;
  if (!the_vector)
    the_vector = new vcl_vector<vgui_toolkit*>;
  return the_vector;
}

vgui_toolkit *vgui_toolkit::lookup(char const *name)
{
  vcl_vector<vgui_toolkit*> *vv = registry();
  for (unsigned int i=0; i<vv->size(); ++i)
    if ((*vv)[i]->name() == name)
      return (*vv)[i];
  vgui_macro_warning << "WARNING : no toolkit with name \'" << name << "\' found." << vcl_endl;
  return 0;
}

//--------------------------------------------------------------------------------

vgui_toolkit::vgui_toolkit()
{
  vbl_trace;
  registry()->push_back(this);
  vbl_trace;
}

vgui_toolkit::~vgui_toolkit()
{
  vbl_trace;

  // deregister
  vcl_vector<vgui_toolkit*>::iterator i = vcl_find(registry()->begin(),
                                                   registry()->end(),
                                                   this);

  vbl_trace;
  assert( i != registry()->end() ); // <-- this is a serious bug
  vbl_trace;
  registry()->erase(i);
  vbl_trace;
}

//
void vgui_toolkit::init(int &, char **) { }


vgui_window *vgui_toolkit::produce_window(int /*width*/,
                                          int /*height*/,
                                          vgui_menu const & /*menubar*/,
                                          char const* /*title*/)
{
  vgui_macro_warning << "no implementation of produce_window supplied" << vcl_endl;
  return 0;
}

vgui_window *vgui_toolkit::produce_window(int /*width*/,
                                          int /*height*/,
                                          char const* /*title*/)
{
  vgui_macro_warning << "no implementation of produce_window supplied" << vcl_endl;
  return 0;
}

vgui_dialog_impl *vgui_toolkit::produce_dialog(char const*)
{
  vgui_macro_warning << "no implementation of produce_dialog supplied" << vcl_endl;
  return 0;
}

void vgui_toolkit::quit()
{
  vgui_macro_warning << "vgui_toolkit::quit() called." << vcl_endl;
  vgui_macro_warning << "calling exit()" << vcl_endl;
  exit(0);
}

void vgui_toolkit::run()
{
  vgui_macro_warning << "no implementation of run supplied" << vcl_endl;
}

void vgui_toolkit::run_one_event()
{
  vgui_macro_warning << "no implementation of run_one_event supplied" << vcl_endl;
}


void vgui_toolkit::run_till_idle()
{
  vgui_macro_warning << "no implementation of run_till_idle supplied" << vcl_endl;
}

void vgui_toolkit::flush()
{
  vgui_macro_warning << "no implementation of vcl_flush supplied" << vcl_endl;
}

void vgui_toolkit::add_event(vgui_event const&)
{
  vgui_macro_warning << "no implementation of add_event supplied" << vcl_endl;
}
