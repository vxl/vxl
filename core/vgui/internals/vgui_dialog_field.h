// This is core/vgui/internals/vgui_dialog_field.h
#ifndef vgui_dialog_field_h_
#define vgui_dialog_field_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   23 Oct 99
// \brief  Represents a field in a dialog.
//
// \verbatim
//  Modifications
//   23-OCT-1999 P.Pritchett - Initial version.
//   10-SEP-2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_string.h>

//: Represents a field in a dialog.
//
//  vgui_dialog_field represents a field in a vgui_dialog. It is the base class for
//  all dialog fields so far. It has abstract functions for synchronising its
//  stored value with a vcl_string
class vgui_dialog_field
{
 public:
  vcl_string label;

  // Constructors/Destructors--------------------------------------------------
  vgui_dialog_field(const char* text) : label(text) {}
  virtual ~vgui_dialog_field() {}

  virtual vcl_string current_value() const = 0;
  virtual bool update_value(const vcl_string& s) = 0;
};

#endif // vgui_dialog_field_h_
