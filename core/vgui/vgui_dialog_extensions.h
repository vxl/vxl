// This is core/vgui/vgui_dialog_extensions.h
#ifndef vgui_dialog_extensions_h_
#define vgui_dialog_extensions_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Gamze Tunali, LEMS, Brown University 
// \date   16 Nov 2007
// \brief  an extension to the vgui_dialog class. vgui_dialog_extensions has two
//         new elements: directory browser and line break. 
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgui/vgui_dialog.h>

class vgui_dialog_extensions_impl;

//: Abstract dialog class
//
// vgui_dialog_extension allows the user to build a dialog from a collection
// of fields. It differs from vgui_dialog by two elements, directory browser
// to be able to choose folders in addition to the files, and line breaks.
// The tableau will be designed so that elements are placed on the same line
// until a line break element is inserted. This allows to create more versatile
// dialogs, by grouping elements one one line.
//
// A field in this context consists of a vcl_string label and a variable.
// The dialog is then posted using the ask() method. If ask returns true then
// any changes to the fields in the dialog are used to update the variables.
// Each vgui_dialog contains an instance of a concrete subclass of
// vgui_dialog_impl. The type of the subclass will be determined by the GUI
// being used.
//
// \par Example
// \code
//   vgui_dialog_extensions params("My params");
//   params.field("Table number", the_table);
//   params.line_break();
//   params.choice("Dressing", "French", "Thousand Island", the_dressing);
//   params.checkbox("Mayo?", has_mayo);
//   params.line_break();
//   params.message("No smoking is allowed in the restaurant!");
//   if (!params.ask())
//     return; // cancelled
//   send_order(the_table, the_dressing, has_mayo);
// \endcode

class vgui_dialog_extensions: public vgui_dialog
{
 public:

  //: Constructor - takes the title of the dialog box.
  vgui_dialog_extensions(const char* name);
 virtual ~vgui_dialog_extensions();

  //: directory browsers
  void dir (const char* label, vcl_string& regexp, vcl_string& dirpath);

  bool ask();
 
  void line_break();

 protected:

  //vgui_dialog_extensions_impl* impl;
};

#endif // vgui_dialog_extensions_h_
