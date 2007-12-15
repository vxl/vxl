// This is core/vgui/internals/vgui_dialog_extensions_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Gamze Tunali, LEMS, Brown University
// \date   16 Nov 2007
// \brief  See vgui_dialog_extensions_impl.h for a description of this file

#include "vgui_dialog_extensions_impl.h"
#include <vgui/internals/vgui_file_field.h>

vgui_dialog_extensions_impl::vgui_dialog_extensions_impl(const char* n)
  : vgui_dialog_impl(n)
{
}

vgui_dialog_extensions_impl::~vgui_dialog_extensions_impl()
{
}

void vgui_dialog_extensions_impl::dir_browser(const char* txt, vcl_string& regexp, vcl_string& val)
{
  vgui_file_field *field = new vgui_file_field(txt, regexp, val);

  element l;
  l.type = dir_bsr;
  l.widget = dir_browser_widget(txt, regexp, val);
  l.field = field;

  elements.push_back(l);
}

void vgui_dialog_extensions_impl::line_break()
{
  element l;
  l.type = line_br;

  elements.push_back(l);
}

void* vgui_dialog_extensions_impl::dir_browser_widget(const char*, vcl_string&, vcl_string&) { return 0; }

