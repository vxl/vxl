// This is core/vgui/vgui_dialog.h
#ifndef vgui_dialog_h_
#define vgui_dialog_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   23 Oct 1999
// \brief  Abstract dialog class.
//
// \verbatim
//  Modifications
//   23-OCT-1999 P.Pritchett - Initial version.
//   25-JAN-2000 K.Y.McGaul - Added message() to allow text messages in dialogs.
//   27-JAN-2000 K.Y.McGaul - Added set_modal() to allow non-modal dialogs.
//   24-FEB-2000 K.Y.McGaul - Added file browser and color chooser.
//   11-JUL-2000 Marko Bacic - Added inline file browser
//   12-JUL-2000 Marko Bacic - Added inline color chooser
//   26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
//   02-JAN-2010 Lianqing Yu - Added controls including push button.
// \endverbatim

#include <string>
#include <vector>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_command_sptr.h>

class vgui_dialog_impl;

//: Abstract dialog class
//
// vgui_dialog allows the user to build a dialog from a collection of fields.
// A field in this context consists of a std::string label and a variable.
// The dialog is then posted using the ask() method. If ask returns true then
// any changes to the fields in the dialog are used to update the variables.
// Each vgui_dialog contains an instance of a concrete subclass of
// vgui_dialog_impl. The type of the subclass will be determined by the GUI
// being used.
//
// \par Example
// \code
//   vgui_dialog params("My params");
//   params.field("Table number", the_table);
//   params.choice("Dressing", "French", "Thousand Island", the_dressing);
//   params.checkbox("Mayo?", has_mayo);
//   params.message("No smoking is allowed in the restaurant!");
//   if (!params.ask())
//     return; // cancelled
//   send_order(the_table, the_dressing, has_mayo);
// \endcode

typedef void (*vgui_dialog_callback)(void const* client_data);
typedef void (*vgui_dialog_callback_no_client_data)();

class vgui_dialog
{
 public:

  //: Constructor - takes the title of the dialog box.
  vgui_dialog(const char* name);
  virtual ~vgui_dialog();

  // A push button is oftern shown with a label and/or a icon/bitmap
  // and a command is executed when the button is clicked.
  void pushbutton(vgui_command_sptr cmnd, const char *label, const void* icon);
  void pushbutton(vgui_dialog_callback_no_client_data f, const char *label, const void* icon);
  void pushbutton(vgui_dialog_callback f, void const *client_data, const char *label, const void* icon);

  void checkbox(const char*, bool&);

  void field(const char*, int&);
  void field(const char* c, unsigned int& v) { field(c,*reinterpret_cast<int*>(&v)); }
  void field(const char*, long&);
  void field(const char*, float&);
  void field(const char*, double&);
  void field(const char*, std::string&);

  //: Multiple choice - with two options.
  void choice(const char* label, const char* option1,
              const char* option2, int& chosen);

  //: Multiple choice - with three options.
  void choice(const char* label, const char* option1,
              const char* option2, const char* option3, int& chosen);

  //: Multiple choice - with the list of options given.
  void choice(const char*, const std::vector<std::string>&, int &);

  void choice(const char*s, const std::vector<std::string>&v, unsigned &r)
  { choice(s,v,*reinterpret_cast<int*>(&r)); }

  //: File browsers
  void file (const char* label, std::string& regexp, std::string& filepath);

  //: inline file browser
  void inline_file(const char* label, std::string& regexp,std::string& filepath);

  //: Color chooser
  void color (const char* label, std::string&);

  //: Inline color chooser
  void inline_color(const char *label, std::string &);

  //: Text message
  void message(const char*);

  void line_break();

  //: Display a tableau in the dialog
  void inline_tableau(const vgui_tableau_sptr tab, unsigned width,
                      unsigned height);

  //: Set the labels on each button, if 0 that button does not appear.
  void set_cancel_button(const char* label);

  //: Set the labels on each button, if 0 that button does not appear.
  void set_ok_button(const char* label);

  //: A "modal" dialog captures all events sent to the application.
  void set_modal(const bool);

  //: Display the dialog box and wait for the users response.
  //  Returns true if the user clicks on the 'OK' button and false if the
  //  user clicks on the 'Cancel' button.
  bool ask();

 protected:
  vgui_dialog() {}
  vgui_dialog_impl* impl;
};

#endif // vgui_dialog_h_
