#ifndef vgui_error_dialog_h_
#define vgui_error_dialog_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_error_dialog
// .INCLUDE vgui/vgui_error_dialog.h
// .FILE vgui_error_dialog.cxx
// Author: awf@robots.ox.ac.uk
// Created: 04 Oct 00

//: Pop up an error dialog, and wait for the user to dismiss it.
void vgui_error_dialog(char const* message);

#endif // vgui_error_dialog_h_
