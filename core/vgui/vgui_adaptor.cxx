// This is core/vgui/vgui_adaptor.cxx
#include "vgui_adaptor.h"
//:
// \file
// \author fsm
// \brief  See vgui_adaptor.h for a description of this file.

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>

#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vgui/internals/vgui_adaptor_tableau.h>


vgui_adaptor *vgui_adaptor::current = 0;
static int adaptor_count = 0;

//-----------------------------------------------------------------------------
//: Constructor - create a default adaptor.
vgui_adaptor::vgui_adaptor()
  : nested_popups(false)
  , default_items(true)
  , use_double_buffering(true)
  , the_tableau(0)
{
  ++adaptor_count;

  the_tableau = new vgui_adaptor_tableau(this);
  the_tableau->ref();

  vgui_menu sub;
  sub.add("Configure", new vgui_command_simple<vgui_adaptor>(this, &vgui_adaptor::config_dialog));
  menu.add("vgui_adaptor", sub);
}

//-----------------------------------------------------------------------------
//: Destructor - quits application if all adaptors have been deleted.
vgui_adaptor::~vgui_adaptor()
{
  set_tableau(0);
  the_tableau->unref();

  --adaptor_count;

  if (adaptor_count == 0) {
    vcl_cerr << "All adaptors have been deleted -- calling vgui::quit()\n";
    vgui::quit();
  }
}

//-----------------------------------------------------------------------------#
//: Get the vgui_tableau associated with this adaptor.
vgui_tableau_sptr vgui_adaptor::get_tableau() const
{
  return the_tableau->get_child();
}

//-----------------------------------------------------------------------------
//: Set the vgui_tableau associated with this adaptor.
void vgui_adaptor::set_tableau(vgui_tableau_sptr const& t)
{
  the_tableau->set_child(t);
}

//-----------------------------------------------------------------------------
//: Returns concatenation of the adaptor's menu with the tableau's menu.
vgui_menu vgui_adaptor::get_total_popup(vgui_popup_params &params) const
{
  // set popup_params members
  params.recurse = true;
  params.nested = nested_popups;
  params.defaults = default_items;

  vgui_menu pop(menu);

  if (menu.size())
    pop.separator();

  vgui_tableau_sptr tab = get_tableau();
  if (tab) {
    vgui_menu tmp;
    tab->get_popup(params, tmp);
    pop.include(tmp);
  }

  return pop;
}

//-----------------------------------------------------------------------------
//: Pops up a dialog for changing the popup modifier and button bindings.
void vgui_adaptor::config_dialog()
{
  // table stores the correspondence between vgui modifiers and pretty names.
  static struct {
    vgui_modifier mod;
    char const *str;
  } mod_table[]={
    {vgui_MODIFIER_NULL, "None"},
    {vgui_CTRL,          "Control"},
    {vgui_SHIFT,         "Shift"},
    {vgui_ALT,           "Alt"},
    {vgui_META,          "Meta"}
  };
  const unsigned num_mods = sizeof(mod_table)/sizeof(mod_table[0]);

  // table stores the correspondence between vgui buttons and pretty names.
  static struct {
    vgui_button but;
    char const *str;
  } but_table[]={
    {vgui_BUTTON_NULL,   "None"},
    {vgui_LEFT,          "Left"},
    {vgui_MIDDLE,        "Middle"},
    {vgui_RIGHT,         "Right"}
  };
  const unsigned num_buts = sizeof(but_table)/sizeof(but_table[0]);

  // get current bindings :
  vgui_modifier popup_modifier_;
  vgui_button popup_button_;
  get_popup_bindings(popup_modifier_, popup_button_);

  // make choice vcl_list, using the ordering in the table,
  // and set the initial value of the modifier index.
  unsigned mod_index = 0;
  vcl_vector<vcl_string> mod_labels;
  for (unsigned i=0; i<num_mods; ++i) {
    if (mod_table[i].mod == popup_modifier_)
      mod_index = i;
    mod_labels.push_back(mod_table[i].str);
  }

  // make choice vcl_list, using the ordering in the table,
  // and set the initial value of the button index.
  unsigned but_index = 0;
  vcl_vector<vcl_string> but_labels;
  for (unsigned i=0; i<num_buts; ++i) {
    if (but_table[i].but == popup_button_)
      but_index = i;
    but_labels.push_back(but_table[i].str);
  }

  // debug
  vcl_cerr << "mod_index " << mod_index << vcl_endl
           << "button_index " << but_index << vcl_endl;

  vgui_dialog mydialog("Adaptor Config");
  mydialog.choice("Popup modifier", mod_labels, mod_index);
  mydialog.choice("Popup button",   but_labels, but_index);
  mydialog.checkbox("Nested popups", nested_popups);
  mydialog.checkbox("Default popup items", default_items);

  if (mydialog.ask()) {
    assert(mod_index < num_mods);
    assert(but_index < num_buts);
    bind_popups(mod_table[mod_index].mod, but_table[but_index].but);
  }
}

//-----------------------------------------------------------------------------
//: Dispatches the given event to the tableau.
//  This method performs various checks which can be performed generically for
//  all tableaux. It is not the responsibility of this method to take care of
//  overlay handling - the derived class must sort that out.
//  The derived class must also ensure that the correct draw buffer is set.
bool vgui_adaptor::dispatch_to_tableau(vgui_event const &e)
{
  vgui_macro_report_errors;

  // if the event is DRAW, DRAW_OVERLAY or RESHAPE
  // then set the glViewport and clear the OpenGL matrices
  if (e.type == vgui_DRAW ||
      e.type == vgui_DRAW_OVERLAY ||
      e.type == vgui_RESHAPE) {
    glViewport(0, 0, get_width(), get_height());

    // set projection matrices.
    vgui_matrix_state::identity_gl_matrices(); // to identity
  }

  vgui_macro_report_errors;

  // sanity check the 'origin' field :
  if (e.origin == 0)
    const_cast<vgui_event&>(e).origin = this;
  else
    assert(e.origin == this);

  vgui_adaptor::current = this;

  // send the event to the tableau :
  bool f = the_tableau->handle(e);
  vgui_macro_report_errors;
  glFlush();
  return f;
}

//-----------------------------------------------------------------------------
vgui_window *vgui_adaptor::get_window() const
{
  vgui_macro_warning << "get_window() not implemented\n";
  return 0;
}

//-----------------------------------------------------------------------------
//: Bind the given modifier/button combination to the popup menu.
void vgui_adaptor::bind_popups(vgui_modifier /*m*/, vgui_button /*b*/)
{
  vgui_macro_warning << "bind_popups() not implemented\n";
}

//-----------------------------------------------------------------------------
void vgui_adaptor::get_popup_bindings(vgui_modifier &, vgui_button &) const
{
  vgui_macro_warning << "get_popup_bindings() not implemented\n";
}

//-----------------------------------------------------------------------------
void vgui_adaptor::swap_buffers()
{
  vgui_macro_warning << "swap_buffers() not implemented\n";
}

//-----------------------------------------------------------------------------
void vgui_adaptor::post_message(char const *, void const *)
{
  vgui_macro_warning << "post_message() not implemented\n";
}

//-----------------------------------------------------------------------------
void vgui_adaptor::make_current()
{
  vgui_macro_warning << "make_current() not implemented\n";
}

//-----------------------------------------------------------------------------
void vgui_adaptor::post_timer(float, int)
{
  vgui_macro_warning << "post_timer(float, int) not implemented\n";
}

//-----------------------------------------------------------------------------
int  vgui_adaptor::post_timer(float t)
{
  static int counter = 0;
  post_timer(t, counter);
  return counter++;
}

//-----------------------------------------------------------------------------
void vgui_adaptor::post_destroy()
{
  vgui_macro_warning << "post_destroy() not implemented\n";
}

void vgui_adaptor::post_idle_request()
{
  // ignore idle processing by default.
}
