// This is oxl/vgui/impl/gtk/vgui_gtk_statusbar.h
#ifndef vgui_gtk_statusbar_h_
#define vgui_gtk_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_gtk_statusbar - Undocumented class FIXME
// .LIBRARY vgui-gtk
// .HEADER vxl Package
// .INCLUDE vgui/impl/gtk/vgui_gtk_statusbar.h
// .FILE vgui_gtk_statusbar.cxx
//
// .SECTION Description:
//
// vgui_gtk_statusbar is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 21 Nov 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <gtk/gtk.h>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

class vgui_gtk_statusbar : public vgui_statusbar
{
 public:

  // Constructors/Destructors--------------------------------------------------

  vgui_gtk_statusbar();
 ~vgui_gtk_statusbar();

  int write(const char* text, int n);
  int write(const char* text);

  GtkWidget *widget;
  vcl_string linebuffer;
  vgui_statusbuf* statusbuf;
  vcl_ostream out;
};

#endif // vgui_gtk_statusbar_h_
