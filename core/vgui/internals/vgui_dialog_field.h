// This is oxl/vgui/internals/vgui_dialog_field.h
#ifndef vgui_dialog_field_h_
#define vgui_dialog_field_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   23 Oct 99
// \brief  Represents a field in a dialog.
//
// \verbatim
//  Modifications
//    23-OCT-1999 P.Pritchett - Initial version.
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

  vgui_dialog_field(const char*);
  virtual ~vgui_dialog_field();

  virtual vcl_string current_value() const = 0;
  virtual bool update_value(const vcl_string& s) = 0;
};

#endif // vgui_dialog_field_h_
