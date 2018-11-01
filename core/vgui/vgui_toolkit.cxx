// This is core/vgui/vgui_toolkit.cxx
//:
// \file
// \author fsm
// \brief  See vgui_toolkit.h for a description of this file.

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "vgui_toolkit.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vul/vul_trace.h>

#include <vgui/vgui_macro.h>
#include <vgui/vgui_window.h>

std::vector<vgui_toolkit*> *vgui_toolkit::registry()
{
  static std::vector<vgui_toolkit*> the_vector;
  return &the_vector;
}

vgui_toolkit *vgui_toolkit::lookup(char const *name)
{
  std::vector<vgui_toolkit*> *vv = registry();
  for (unsigned int i=0; i<vv->size(); ++i)
    if ((*vv)[i]->name() == name)
      return (*vv)[i];
  vgui_macro_warning << "WARNING : no toolkit with name \'" << name << "\' found.\n";
  return nullptr;
}

//--------------------------------------------------------------------------------

vgui_toolkit::vgui_toolkit()
{
  vul_trace;
  registry()->push_back(this);
  vul_trace;
}

vgui_toolkit::~vgui_toolkit()
{
  vul_trace;

  // deregister
  std::vector<vgui_toolkit*>::iterator i = std::find(registry()->begin(),
                                                   registry()->end(),
                                                   this);

  vul_trace;
  assert( i != registry()->end() ); // <-- this is a serious bug
  vul_trace;
  registry()->erase(i);
  vul_trace;
}

//
void vgui_toolkit::init(int &, char **) { }


vgui_window *vgui_toolkit::produce_window(int /*width*/,
                                          int /*height*/,
                                          vgui_menu const & /*menubar*/,
                                          char const* /*title*/)
{
  vgui_macro_warning << "no implementation of produce_window supplied\n";
  return nullptr;
}

vgui_window *vgui_toolkit::produce_window(int /*width*/,
                                          int /*height*/,
                                          char const* /*title*/)
{
  vgui_macro_warning << "no implementation of produce_window supplied\n";
  return nullptr;
}

vgui_dialog_impl *vgui_toolkit::produce_dialog(char const*)
{
  vgui_macro_warning << "no implementation of produce_dialog supplied\n";
  return nullptr;
}

vgui_dialog_extensions_impl *vgui_toolkit::produce_dialog_extension(char const*)
{
  vgui_macro_warning << "no implementation of produce_dialog supplied\n";
  return nullptr;
}

void vgui_toolkit::quit()
{
  vgui_macro_warning << "vgui_toolkit::quit() called.\n"
                     << "calling exit()\n";
  std::exit(0);
}

void vgui_toolkit::run()
{
  vgui_macro_warning << "no implementation of run supplied\n";
}

void vgui_toolkit::run_one_event()
{
  vgui_macro_warning << "no implementation of run_one_event supplied\n";
}


void vgui_toolkit::run_till_idle()
{
  vgui_macro_warning << "no implementation of run_till_idle supplied\n";
}

void vgui_toolkit::flush()
{
  vgui_macro_warning << "no implementation of flush supplied\n";
}

void vgui_toolkit::add_event(vgui_event const&)
{
  vgui_macro_warning << "no implementation of add_event supplied\n";
}

void vgui_toolkit::uninit()
{
}
