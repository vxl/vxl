// This is core/vgui/vgui_wrapper_tableau.cxx
#include <iostream>
#include "vgui_wrapper_tableau.h"
//:
// \file
// \author fsm
// \brief  See vgui_wrapper_tableau.h for a description of this file.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_event.h>

//--------------------------------------------------------------------------------

vgui_wrapper_tableau::vgui_wrapper_tableau()
  : child(this)
{
}

vgui_wrapper_tableau::vgui_wrapper_tableau(vgui_tableau_sptr const&n)
  : child(this, n)
{
}

vgui_wrapper_tableau::~vgui_wrapper_tableau()
{
}

std::string vgui_wrapper_tableau::type_name() const
{
  return "vgui_wrapper_tableau";
}

std::string vgui_wrapper_tableau::pretty_name() const
{
  return child ? std::string(type_name() + "[" + child->pretty_name() + "]") : std::string("(null)");
}

std::string vgui_wrapper_tableau::file_name() const
{
  return child ? child->file_name() : std::string("(null)");
}

bool vgui_wrapper_tableau::add_child(vgui_tableau_sptr const& c)
{
  if (child) {
    std::cerr << __FILE__ " cannot add child " << c << "; only one child allowed\n";
    return false;
  }
  else {
    child.assign(c);
    return true;
  }
}

bool vgui_wrapper_tableau::remove_child(vgui_tableau_sptr const& c)
{
  if (child.child() != c) {
    std::cerr << __FILE__ " no such child : " << c << std::endl;
    return false;
  }
  else {
    child.assign(nullptr);
    return true;
  }
}

bool vgui_wrapper_tableau::handle(vgui_event const& e)
{
  return child && child->handle(e);
}

bool vgui_wrapper_tableau::get_bounding_box(float low[3], float high[3]) const
{
  return child && child->get_bounding_box(low, high);
}

//--------------------------------------------------------------------------------
