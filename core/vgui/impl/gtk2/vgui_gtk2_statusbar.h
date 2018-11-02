// This is core/vgui/impl/gtk2/vgui_gtk2_statusbar.h
#ifndef vgui_gtk2_statusbar_h_
#define vgui_gtk2_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  The GTK implementation of vgui_statusbar.
// \author Robotics Research Group, University of Oxford
// \date   21 Nov 99
//
//  Contains class  vgui_gtk2_statusbar
//
// \verbatim
//  Modifications
//   13-SEP-2002 K.Y.McGaul - Changed to Doxygen style documentation.
// \endverbatim

#include <string>
#include <ostream>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <gtk/gtk.h>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

//: The GTK implementation of vgui_statusbar.
//  The GTK status bar for the main window of the application.
class vgui_gtk2_statusbar : public vgui_statusbar
{
 public:
  //: Constructor, creates a GTK status bar and displays it.
  vgui_gtk2_statusbar();

  //: Destructor.
 ~vgui_gtk2_statusbar();

  //: Append given text (of given length) to the status bar.
  int write(const char* text, int n);

  //: Append given text to the status bar.
  int write(const char* text);

  GtkWidget *widget;
  std::string linebuffer;
  vgui_statusbuf* statusbuf;
  std::ostream out;
};

#endif // vgui_gtk2_statusbar_h_
