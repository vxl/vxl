// This is oxl/xcv/xcv_tjunction.cxx
#include "xcv_tjunction.h"
//:
//  \file
// See xcv_tjunction.h for a description of this file.
//
// \author  K.Y.McGaul

xcv_tjunction::xcv_tjunction(xcv_mview_manager* xmm)
  : vgui_tableau()
  , xcv_mgr(xmm)
  , child_tab(this, nullptr)
{
}

xcv_tjunction::~xcv_tjunction() { }

void xcv_tjunction::set_child(vgui_tableau_sptr const& ct)
{
  child_tab.assign(ct);
}

std::string xcv_tjunction::type_name() const
{
  return std::string("xcv_tjunction");
}

std::string xcv_tjunction::file_name() const
{
  return std::string("(none)");
}

std::string xcv_tjunction::pretty_name() const
{
  return std::string("xcv_tjunction");
}

//------------------------------------------------------------------------------
//: Handle events by passing them to the manager and the child tableau.
//------------------------------------------------------------------------------
bool xcv_tjunction::handle(const vgui_event &e)
{
  bool h = child_tab.handle(e);
  xcv_mgr->handle_tjunction_event(e, this);
  return h;
}
