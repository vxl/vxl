// This is core/vgui/vgui_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_window.h for a description of this file.

#include "vgui_window.h"
#include <vcl_iostream.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_statusbar.h>

//------------------------------------------------------------------------------

vgui_window::vgui_window()
{
}

vgui_window::~vgui_window()
{
}

//------------------------------------------------------------------------------

void vgui_window::set_menubar(vgui_menu const &)
{
  vgui_macro_warning << "set_menubar() not implemented for this window\n";
}

//------------------------------------------------------------------------------

void vgui_window::set_adaptor(vgui_adaptor*)
{
  vgui_macro_warning << "set_adaptor() not implemented for this window\n";
}

vgui_adaptor* vgui_window::get_adaptor()
{
  vgui_macro_warning << "get_adaptor() not implemented for this window\n";
  return 0;
}

vgui_statusbar* vgui_window::get_statusbar()
{
  vgui_macro_warning << "get_statusbar() not implemented for this window\n";
  return 0;
}

//------------------------------------------------------------------------------


void vgui_window::show()
{
  vgui_macro_warning << "vgui_window::show() dummy implementation\n";
}

void vgui_window::enable_hscrollbar(bool)
{
  vgui_macro_warning << "vgui_window::enable_hscrollbar() dummy implementation\n";
}

void vgui_window::enable_vscrollbar(bool)
{
  vgui_macro_warning << "vgui_window::enable_vscrollbar() dummy implementation\n";
}

void vgui_window::hide()
{
  vgui_macro_warning << "vgui_window::show() dummy implementation\n";
}

void vgui_window::iconify()
{
  vgui_macro_warning << "vgui_window::hide() dummy implementation\n";
}

void vgui_window::reshape(unsigned,unsigned)
{
  vgui_macro_warning << "vgui_window::reshape() dummy implementation\n";
}

void vgui_window::reposition(int,int)
{
  vgui_macro_warning << "vgui_window::reposition() dummy implementation\n";
}

void vgui_window::set_title(vcl_string const&)
{
  vgui_macro_warning << "vgui_window::set_title() dummy implementation\n";
}

int vgui_window::set_hscrollbar(int  /*pos*/)
{
  vgui_macro_warning << "vgui_window::set_hscrollbar() dummy implementation\n";
  return 0;
}

int vgui_window::set_vscrollbar(int  /*pos*/)
{
  vgui_macro_warning << "vgui_window::set_vscrollbar() dummy implementation\n";
  return 0;
}

//------------------------------------------------------------------------------
