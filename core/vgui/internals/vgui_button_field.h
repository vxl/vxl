// This is core/vgui/internals/vgui_button_field.h
#ifndef vgui_button_field_h_
#define vgui_button_field_h_

//:
// \file
// \author  Lianqing Yu
// \brief   button control in a dialog.
//
// \verbatim
//  Modifications
//   02 Jan. 2010 initial version
// \endverbatim

#include "vgui_dialog_field.h"
#include <vgui/vgui_command_sptr.h>

//: button control in a dialog.
//
// vgui_button_field is a dialog field implementation that holds
// information of a button control. Usually it contains a button label
// and/or an icon/bitmap shown on the button.
class vgui_button_field : public vgui_dialog_field
{
 public:
  vgui_button_field(vgui_command_sptr c, const char *txt)
  : vgui_dialog_field(txt ? txt : ""), cmnd(c) {}

  ~vgui_button_field() {}

  std::string current_value() const { return "OK"; }
  bool update_value(const std::string& s) { return true; }

  // Pointer to the command to be executed.
  vgui_command_sptr cmnd;
};

#endif // vgui_button_field_h_
