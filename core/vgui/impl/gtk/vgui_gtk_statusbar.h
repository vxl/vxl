// This is core/vgui/impl/gtk/vgui_gtk_statusbar.h
#ifndef vgui_gtk_statusbar_h_
#define vgui_gtk_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Robotics Research Group, University of Oxford
// \date   21 Nov 99
// \brief  The GTK implementation of vgui_statusbar.
//
//  Contains  classes:  vgui_gtk_statusbar
// 
// \verbatim
//  Modifications:
//    13-SEP-2002 K.Y.McGaul - Changed to Doxygen style documentation.
// \endverbatim

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <gtk/gtk.h>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

//: The GTK implementation of vgui_statusbar.
//  The GTK status bar for the main window of the application.
class vgui_gtk_statusbar : public vgui_statusbar
{
 public:
  //: Constructor, creates a GTK status bar and displays it.
  vgui_gtk_statusbar();
 
  //: Destructor.
 ~vgui_gtk_statusbar();

  //: Append given text (of given length) to the status bar.
  int write(const char* text, int n);

  //: Append given text to the status bar.
  int write(const char* text);

  GtkWidget *widget;
  vcl_string linebuffer;
  vgui_statusbuf* statusbuf;
  vcl_ostream out;
};

#endif // vgui_gtk_statusbar_h_
