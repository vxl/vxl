#ifndef vgui_dialog_h_
#define vgui_dialog_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_dialog - Abstract dialog class
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_dialog.h
// .FILE vgui_dialog.cxx
//
// .SECTION Description
//
// vgui_dialog allows the user to build a dialog from a collection of fields.
// A field in this context consists of a vcl_string label and a variable. 
// The dialog is then posted using the ask() method. If ask returns true then any changes
// to the fields in the dialog are used to update the variables.
// Each vgui_dialog contains an instance of a concrete subclass of vgui_dialog_impl.
// The type of the subclass will be determined by the GUI being used.
//
// .SECTION Example
//
//   vgui_dialog params("My params");
//   params.field("Table number", the_table);
//   params.choice("Dressing", "French", "Thousand Island", the_dressing);
//   params.checkbox("Mayo?", has_mayo);
//   params.message("No smoking is allowed in the restaurant!");
//   if (!params.ask())
//     return; // cancelled
//   send_order(the_table, the_dressing, has_mayo);
//
// .SECTION Author
//              Philip C. Pritchett, 23 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//   K.Y.McGaul 25-JAN-00   Added message function to allow text messages in dialogs.
//   K.Y.McGaul 27-JAN-00   Added set_modal function to allow non-modal dialogs.
//   K.Y.McGaul 24-FEB-00   Added file browser and color chooser.
//   Marko Bacic 11-JUL-00  Added inline file browser
//   Marko Bacic 12-JUL-00  Added inline color chooser
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgui/vgui_tableau.h>
//class vgui_dialog_field;
class vgui_dialog_impl;

class vgui_dialog 
{
public:
    
  vgui_dialog(const char* name);
 ~vgui_dialog();

  void checkbox(const char*, bool&);

  void field(const char*, int&);
  void field(const char*, long&);
  void field(const char*, float&);
  void field(const char*, double&);
  void field(const char*, vcl_string&);

  // multiple choice
  void choice(const char* label, const char* option1, const char* option2, int& chosen);
  void choice(const char* label, const char* option1, const char* option2, const char* option3, int& chosen);
  void choice(const char*, const vcl_vector<vcl_string>&, int &);  // get index in vcl_vector of strings.
  void choice(const char*s, const vcl_vector<vcl_string>&v, unsigned &r) { choice(s,v,*reinterpret_cast<int*>(&r)); }

  //void choices(const vcl_vector<vcl_string> &,vcl_vector<int> &); // get indices in vcl_vector of strings.

  //: File browsers
  void file (const char* label, vcl_string& regexp, vcl_string& filepath);
  //void files(const char *label,vcl_string& regexp, vcl_vector<vcl_string> &);
  //: inline file browser
  void inline_file(const char* label, vcl_string& regexp,vcl_string& filepath);
  //: Color chooser
  void color (const char* label, vcl_string&);
  //: Inline color chooser
  void inline_color(const char *label, vcl_string &);
  //: Text message
  void message(const char*);

  //: Display a tableau in the dialog
  void inline_tableau(const vgui_tableau_sptr tab, unsigned width, unsigned height);

  // Set the labels on each button, if 0 that button does not appear.
  void set_cancel_button(const char* label);
  void set_ok_button(const char* label);

  // A "modal" dialog captures all events sent to the 
  // application.
  void set_modal(const bool);

  bool ask();

protected:
  vgui_dialog_impl* impl;
};

#endif // vgui_dialog_h_
