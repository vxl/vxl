// This is core/vgui/internals/vgui_dialog_impl.h
#ifndef vgui_dialog_impl_h_
#define vgui_dialog_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief vgui_dialog_impl is the abstract base class for dialog implementation.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   25 Oct 99
//
// \verbatim
//  Modifications
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
  //: Constructor - create an empty dialog with the given title.
  vgui_dialog_impl(const char* dialog_name);

  //: Destructor - delete this dialog box.
  virtual ~vgui_dialog_impl();

  //: Add a boolean field to the dialog box.
  void bool_field(const char*, bool&);

  //: Add an integer field to the dialog box.
  void int_field(const char*, int&);

  //: Add a long integer field to the dialog box.
  void long_field(const char*, long&);

  //: Add a float field to the dialog box.
  void float_field(const char*, float&);

  //: Add a double field to the dialog box.
  void double_field(const char*, double&);

  //: Add a vcl_string field to the dialog box.
  void string_field(const char*, vcl_string&);

  //: Add a choice (selection box) to the dialog box.
  void choice_field(const char*, const vcl_vector<vcl_string>&, int&);

  //: Add a text message to the dialog box.
  void text_message(const char*);

  //: Add a popup file browser to the dialog box.
  void file_browser(const char*, vcl_string&, vcl_string&);

  //: Add an inline file browser to the dialog box.
  void inline_file_browser(const char *, vcl_string&, vcl_string&);

  //: Add a popup colour chooser to the dialog box.
  void color_chooser(const char*, vcl_string&);

  //: Add an inline colour chooser to the dialog box.
  void inline_color_chooser(const char*, vcl_string&);

  //: Add a tableau (OpenGL area) to the dialog box.
  void inline_tab(const vgui_tableau_sptr tab, unsigned width, unsigned height);

  //: Pointer to a GUI widget for a bool field.
  virtual void* bool_field_widget(const char*, bool&);

  //: Pointer to a GUI widget for a integer field.
  virtual void* int_field_widget(const char*, int&);

  //: Pointer to a GUI widget for a long integer field.
  virtual void* long_field_widget(const char*, long&);

  //: Pointer to a GUI widget for a float field.
  virtual void* float_field_widget(const char*, float&);

  //: Pointer to a GUI widget for a double field.
  virtual void* double_field_widget(const char*, double&);

  //: Pointer to a GUI widget for a string field.
  virtual void* string_field_widget(const char*, vcl_string&);

  //: Pointer to a GUI widget for a choice field.
  virtual void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);

  //: Pointer to a GUI widget for a text message.
  virtual void* text_message_widget(const char*);

  //: Pointer to a GUI widget for a file browser.
  virtual void* file_browser_widget(const char*, vcl_string&, vcl_string&);

  //: Pointer to a GUI widget for an inline file browser.
  virtual void* inline_file_browser_widget(const char *,vcl_string&, vcl_string&);

  //: Pointer to a GUI widget for a colour chooser.
  virtual void* color_chooser_widget(const char*, vcl_string&);

  //: Pointer to a GUI widget for an inline colour chooser.
  virtual void* inline_color_chooser_widget(const char *,vcl_string&);

  //: Pointer to a GUI widget for a tableau (OpenGL area).
  virtual void* inline_tableau_widget(const vgui_tableau_sptr tab, unsigned width, unsigned height);

  //: Set the modality of the dialog box.
  //  True makes the dialog modal (i.e. the dialog 'grabs' all events) and
  //  this is the default.  WARNING: It is dangerous to make a dialog that
  //  changes data non-modal, only messages should be non-modal.
  virtual void modal(bool);

  //: Set the text on the cancel button.
  virtual void set_cancel_button(const char* msg) { cancel_button_text_ = msg ? msg : ""; }

  //: Set the text on the OK button.
  virtual void set_ok_button(const char* msg) { ok_button_text_ = msg?msg:""; }

  //: Display the dialog box and collect data from the user.
  virtual bool ask() = 0;

  //: Enum of possible element types.
  enum element_type {bool_elem, int_elem, long_elem, float_elem,
                     double_elem, string_elem, choice_elem, text_msg,
                     file_bsr, color_csr,inline_file_bsr,inline_color_csr,
                     inline_tabl, unknown};

  //: Data associated with each field in the dialog box.
  //  The representation of a dialog box in vgui is simply as a list
  //  of these elements.
  struct element
  {
    //: What type of field this is (int, bool, file browser, etc)
    element_type type;
    //: A pointer to a GUI widget for this field, if one exists.
    //  This is null in most cases since it is easier to construct
    //  widgets as we need them, except perhaps for something
    //  complicated like a file browser or colour chooser. The GUI
    //  implementation is completely responsible for this pointer
    //  (i.e. ensuring memory deallocation when the dialog closes,
    //  etc.)
    void *widget;
    //: Field to collect data from the user.
    // The derived GUI implementation should not delete these.
    vgui_dialog_field *field;

    element() : type(unknown), widget(0), field(0) {}
  };

 protected:
  vcl_string name;
  vcl_vector<element> elements;
  vcl_string cancel_button_text_;
  vcl_string ok_button_text_;
};

#endif // vgui_dialog_impl_h_
