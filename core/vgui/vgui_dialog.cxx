// This is core/vgui/vgui_dialog.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   23 Oct 1999
// \brief  See vgui_dialog.h for a description of this file.

#include "vgui_dialog.h"
#include <vgui/vgui.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_command.h>
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

void vgui_dialog::pushbutton(vgui_command_sptr cmnd, const char *label, const void* icon)
{
  if (impl) impl->pushbutton_field(cmnd, label, icon);
}

void vgui_dialog::pushbutton(vgui_dialog_callback_no_client_data f, const char *label, const void* icon)
{
  vgui_command* cfunc = new vgui_command_cfunc(f);
  pushbutton(cfunc, label, icon);
}

void vgui_dialog::pushbutton(vgui_dialog_callback f, void const *client_data, const char *label, const void* icon)
{
  vgui_command* cfunc = new vgui_command_cfunc(f, client_data);
  pushbutton(cfunc, label, icon);
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

void vgui_dialog::field(const char* txt, std::string& v)
{
  if (impl) impl->string_field(txt, v);
}

void vgui_dialog::choice(const char* txt, const std::vector<std::string>& labels, int& v)
{
  if (impl) impl->choice_field(txt, labels, v);
}

void vgui_dialog::choice(const char* label, const char* option1, const char* option2, int& chosen)
{
  std::vector<std::string> strs;
  strs.push_back(option1);
  strs.push_back(option2);
  choice(label, strs, chosen);
}

void vgui_dialog::choice(const char* label, const char* option1, const char* option2, const char* option3, int& chosen)
{
  std::vector<std::string> strs;
  strs.push_back(option1);
  strs.push_back(option2);
  strs.push_back(option3);
  choice(label, strs, chosen);
}

void vgui_dialog::file(const char* label, std::string& regexp, std::string& v)
{
  if (impl) impl->file_browser(label, regexp, v);
}

void vgui_dialog::inline_file(const char* label,std::string& regexp,
                              std::string& v)
{
  if (impl) impl->inline_file_browser(label, regexp, v);
}

void vgui_dialog::color(const char* label, std::string& v)
{
  if (impl) impl->color_chooser(label, v);
}

void vgui_dialog::inline_color(const char* label, std::string& v)
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

void vgui_dialog::line_break()
{
  if (impl) impl->line_break();
}
