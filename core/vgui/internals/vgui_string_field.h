// This is oxl/vgui/internals/vgui_string_field.h
#ifndef vgui_string_field_h_
#define vgui_string_field_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_string_field - vcl_string field in a dialog
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/internals/vgui_string_field.h
// .FILE internals/vgui_string_field.cxx
//
// .SECTION Description
//
// vgui_string_field is a dialog field implementation that holds a vcl_string.
//
// .SECTION Author
//              Philip C. Pritchett, 24 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include "vgui_dialog_field.h"

class vgui_string_field : public vgui_dialog_field
{
 public:
  vgui_string_field(const char *text, vcl_string &str);
  ~vgui_string_field();

  vcl_string current_value() const;
  bool update_value(const vcl_string &);

 private:
  vcl_string &var;
};

#endif // vgui_string_field_h_
