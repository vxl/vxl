// This is core/vgui/impl/gtk2/vgui_gtk2_dialog_impl.h
#ifndef vgui_gtk2_dialog_impl_h_
#define vgui_gtk2_dialog_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  The GTK implementation of vgui_dialog_impl.
// \author Robotics Research Group, University of Oxford
// \date   28 Dec 99
//
//  Contains class  vgui_gtk2_dialog_impl
//
// \verbatim
//  Modifications
//   K.Y.McGaul  25-JAN-00  Moved all field functions to vgui_dialog_impl.
//                          Added choice_field_widget function.
//   Marko Bacic 11-JUL-00  Added support for inline file dialog box
//   Marko Bacic 12-JUL-00  Added support for inline color chooser box
//   Marko Bacic 14-JUL-00  Fixed misalignment of entry boxes
//   Marko Bacic 20-JUL-00  Fixed bug in inline file dialog box. Now returns
//                          the full pathname
//   K.Y.McGaul  31-MAY-01  Added set_ok/cancel_button.
// \endverbatim

#include <vgui/internals/vgui_dialog_impl.h>
#include <gtk/gtk.h>
class vgui_tableau_sptr;

//: Specialization of vgui_dialog_impl for GTK. Creates a GTK dialog box.
class vgui_gtk2_dialog_impl : public vgui_dialog_impl
{
 public:
  vgui_gtk2_dialog_impl(const char* name);
  ~vgui_gtk2_dialog_impl();

  void* choice_field_widget(const char*, const vcl_vector<vcl_string>&, int&);
  void* inline_tableau_widget(const vgui_tableau_sptr tab, unsigned width, unsigned height);

  //--------------------------------------------------------------------------------
  //: Changes the modality of the dialog.
  //  True makes the dialog modal (i.e. the dialog 'grabs' all events), this is the default.
  //  False makes the dialog non-modal.
  void modal(bool m);

  void set_ok_button(const char* txt);
  void set_cancel_button(const char* txt);

  bool ask();

 public:
  // Status of this dialog box.
  enum status_type { BUSY,     // user is doing stuff
                     OK,       // okay button pressed
                     CANCEL,   // cancel button pressed
                     CLOSE }; // dialog window closed

 private:
  GtkWidget* dialog_window;
  GtkWidget* vbox;

  status_type dialog_status_;

  vcl_string ok_text;
  vcl_string cancel_text;
  vcl_string title;
};

#endif // vgui_gtk2_dialog_impl_h_
