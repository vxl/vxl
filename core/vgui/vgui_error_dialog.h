// This is oxl/vgui/vgui_error_dialog.h

//:
// \file
// \author awf@robots.ox.ac.uk
// \date   04 Oct 00

#ifndef vgui_error_dialog_h_
#define vgui_error_dialog_h_
#ifdef __GNUC__
#pragma interface
#endif

//: Pop up an error dialog, and wait for the user to dismiss it.
void vgui_error_dialog(char const* message);

#endif // vgui_error_dialog_h_
