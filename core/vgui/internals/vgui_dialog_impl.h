// This is oxl/vgui/internals/vgui_dialog_impl.h
#ifndef vgui_dialog_impl_h_
#define vgui_dialog_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   25 Oct 99
// \brief vgui_dialog_impl is the abstract base class for dialog implementation.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul  25-JAN-00    Moved field functions to this class to save repetition.
//                            Added virtual ..._widget functions.
//                            Added text_message function.
//   K.Y.McGaul  27-JAN-00    Added modal function.
//   Marko Bacic 11-JUL-00    Added support for inline file browser
//   Marko Bacic 12-JUL-00    Added support for inline color chooser
//   Joris S.    09-NOV-00    Fixed weird color browser things
//   K.Y.McGaul  22-MAY-01    Added tableau field.
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgui/vgui_tableau.h>

class vgui_dialog_field;

//: vgui_dialog_impl is the abstract base class for dialog implementation.
//
//  It contains methods for adding fields corresponding to those in
//  vgui_dialog. It also contains a vcl_vector of elements which are tuples of
//  vgui_dialog_field and a variable indicating what type of field they are. The
//  elements also contain void* for implementors to add any gui specific
//  information/class to the element.
class vgui_dialog_impl
{
 public:
  vgui_dialog_impl(const char* dialog_name);
  virtual ~vgui_dialog_impl();

  void bool_field(const char*, bool&);
  void int_field(const char*, int&);
  void long_field(const char*, long&);
  void float_field(const char*, float&);
  void double_field(const char*, double&);
  void string_field(const char*, vcl_string&);
  void choice_field(const char*, const vcl_vector<vcl_string>&, int&);
  void text_message(const char*);
  void file_browser(const char*, vcl_string&, vcl_string&);
  void inline_file_browser(const char *, vcl_string&, vcl_string&);
  void color_chooser(const char*, vcl_string&);
  void inline_color_chooser(const char*, vcl_string&);
  void inline_tab(const vgui_tableau_sptr tab, unsigned width, unsigned height);

  virtual void* bool_field_widget(const char*, bool&);
  virtual void* int_field_widget(const char*, int&);
  virtual void* long_field_widget(const char*, long&);
  virtual void* float_field_widget(const char*, float&);
  virtual void* double_field_widget(const char*, double&);
  virtual void* string_field_widget(const char*, vcl_string&);
  virtual void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);
  virtual void* text_message_widget(const char*);
  virtual void* file_browser_widget(const char*, vcl_string&, vcl_string&);
  virtual void* inline_file_browser_widget(const char *,vcl_string&, vcl_string&);
  virtual void* color_chooser_widget(const char*, vcl_string&);
  virtual void* inline_color_chooser_widget(const char *,vcl_string&);
  virtual void* inline_tableau_widget(const vgui_tableau_sptr tab, unsigned width,
    unsigned height);
  virtual void modal(const bool);
  virtual void set_cancel_button(const char* msg);
  virtual void set_ok_button(const char* msg);

  virtual bool ask() = 0;

  enum element_type {bool_elem, int_elem, long_elem, float_elem,
                     double_elem, string_elem, choice_elem, text_msg,
                     file_bsr, color_csr,inline_file_bsr,inline_color_csr,
                     inline_tabl};

  struct element
  {
    element_type type;
    void *widget;
    vgui_dialog_field *field;
  };

 protected:
  vcl_string name;
  vcl_vector<element> elements;
  vcl_string cancel_button_text_;
  vcl_string ok_button_text_;
};

#endif // vgui_dialog_impl_h_
