// This is oxl/vgui/internals/vgui_file_field.h
#ifndef vgui_file_field_h_
#define vgui_file_field_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  VGG, Oxford University
// \brief   File browser in a dialog.

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
  vgui_file_field(const char *text, vcl_string&, vcl_string&);
  ~vgui_file_field();

  vcl_string current_value() const;
  bool update_value(const vcl_string &);

 private:
  vcl_string &var;
  vcl_string &regexp;
};

#endif // vgui_file_field_h_
