// This is core/vgui/vgui_dialog.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   23 Oct 99
// \brief  See vgui_dialog.h for a description of this file.


#include "vgui_dialog.h"
#include <vgui/vgui.h>

#include <vgui/internals/vgui_dialog_impl.h>

//-----------------------------------------------------------------------------
//
// Default Constructor
//

vgui_dialog::vgui_dialog(const char* name)
{
  // will eventually be
  impl = vgui::produce_dialog(name);
}


vgui_dialog::~vgui_dialog()
{
  delete impl;
}


bool vgui_dialog::ask()
{
  if (impl)
    return impl->ask();

  return false;
}

void vgui_dialog::checkbox(const char* txt, bool& v)
{
  if (impl) impl->bool_field(txt, v);
}


void vgui_dialog::field(const char* txt, int& v)
{
  if (impl) impl->int_field(txt, v);
}


void vgui_dialog::field(const char* txt, long& v)
{
  if (impl) impl->long_field(txt, v);
}

void vgui_dialog::field(const char* txt, float& v)
{
  if (impl) impl->float_field(txt, v);
}

void vgui_dialog::field(const char* txt, double& v)
{
  if (impl) impl->double_field(txt, v);
}

void vgui_dialog::field(const char* txt, vcl_string& v)
{
  if (impl) impl->string_field(txt, v);
}

void vgui_dialog::choice(const char* txt, const vcl_vector<vcl_string>& labels, int& v)
{
  if (impl) impl->choice_field(txt, labels, v);
}

void vgui_dialog::choice(const char* label, const char* option1, const char* option2, int& chosen)
{
  vcl_vector<vcl_string> strs;
  strs.push_back(option1);
  strs.push_back(option2);
  choice(label, strs, chosen);
}

void vgui_dialog::choice(const char* label, const char* option1, const char* option2, const char* option3, int& chosen)
{
  vcl_vector<vcl_string> strs;
  strs.push_back(option1);
  strs.push_back(option2);
  strs.push_back(option3);
  choice(label, strs, chosen);
}

void vgui_dialog::file(const char* label, vcl_string& regexp, vcl_string& v)
{
  if (impl) impl->file_browser(label, regexp, v);
}

void vgui_dialog::inline_file(const char* label,vcl_string& regexp,
                              vcl_string& v)
{
  if (impl) impl->inline_file_browser(label, regexp, v);
}

void vgui_dialog::color(const char* label, vcl_string& v)
{
  if (impl) impl->color_chooser(label, v);
}

void vgui_dialog::inline_color(const char* label, vcl_string& v)
{
  if (impl) impl->inline_color_chooser(label, v);
}

void vgui_dialog::message(const char* txt)
{
  if (impl) impl->text_message(txt);
}

void vgui_dialog::inline_tableau(const vgui_tableau_sptr tab, unsigned width,
                                 unsigned height)
{
  if (impl) impl->inline_tab(tab, width, height);
}

void vgui_dialog::set_cancel_button(const char* txt)
{
  if (impl) impl->set_cancel_button(txt);
}

void vgui_dialog::set_ok_button(const char* txt)
{
  if (impl) impl->set_ok_button(txt);
}

void vgui_dialog::set_modal(const bool is_modal)
{
  if (impl) impl->modal(is_modal);
}
