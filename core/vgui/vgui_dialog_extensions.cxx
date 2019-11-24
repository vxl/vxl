// This is core/vgui/vgui_dialog_extensions.cxx
//:
// \file
// \author Gamze Tunali, LEMS, Brown University
// \date   16 Nov 2007
// \brief  See vgui_dialog_extensions.h for a description of this file.

#include "vgui_dialog_extensions.h"
#include "vgui/vgui.h"
#include <vgui/internals/vgui_dialog_extensions_impl.h>

//-----------------------------------------------------------------------------
//
// Default Constructor
//

vgui_dialog_extensions::vgui_dialog_extensions(const char* name)
: vgui_dialog()
{
  impl = vgui::produce_extension_dialog(name);
}


vgui_dialog_extensions::~vgui_dialog_extensions()
{
  delete impl;
  impl = nullptr;//so base class doesn't crash
}

bool vgui_dialog_extensions::ask()
{
  if (impl) {
    vgui_dialog_extensions_impl* my_impl = static_cast<vgui_dialog_extensions_impl*> (impl);
    return my_impl->ask();
  }
  return false;
}

void vgui_dialog_extensions::dir(const char* label, std::string& regexp, std::string& v)
{
  if (impl) {
    vgui_dialog_extensions_impl* my_impl = static_cast<vgui_dialog_extensions_impl*> (impl);
    my_impl->dir_browser(label, regexp, v);
  }
}

void vgui_dialog_extensions::line_break()
{
   if (impl) {
    vgui_dialog_extensions_impl* my_impl = static_cast<vgui_dialog_extensions_impl*> (impl);
    my_impl->line_break();
   }
}
