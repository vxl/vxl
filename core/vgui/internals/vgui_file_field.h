// This is core/vgui/internals/vgui_file_field.h
#ifndef vgui_file_field_h_
#define vgui_file_field_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  K.Y.McGaul, VGG, Oxford University
// \brief   File browser in a dialog.
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_string.h>
#include "vgui_dialog_field.h"

//: File browser in a dialog.
//
// vgui_file_field is a dialog field implementation that holds a string
// which contains a file name.  In most implementations this will be filled
// in by a file browser, but the default is just a text entry box.
class vgui_file_field : public vgui_dialog_field
{
 public:
  vgui_file_field(const char *txt, vcl_string& regex, vcl_string& var_to_modify)
  : vgui_dialog_field(txt), var_(var_to_modify), regexp_(regex) {}

  ~vgui_file_field() {}

  vcl_string current_value() const { return var_; }
  bool update_value(vcl_string const& s) { var_ = s; return true; }

 private:
  vcl_string &var_;
  vcl_string &regexp_;
};

#endif // vgui_file_field_h_
