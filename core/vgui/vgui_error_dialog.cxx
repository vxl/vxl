// This is ./oxl/vgui/vgui_error_dialog.cxx

//:
// \file
// \author VGG, Oxford University
// \brief  See vgui_error_dialog.h for a description of this file.


#include "vgui_error_dialog.h"

#include <vgui/vgui_dialog.h>

void vgui_error_dialog(char const* msg)
{
  vgui_dialog d("ERROR");
  d.message(msg);
  d.set_cancel_button("");
  d.ask();
}
