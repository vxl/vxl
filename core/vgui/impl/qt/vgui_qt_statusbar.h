#ifndef VGUI_QT_STATUSBAR_H_
#define VGUI_QT_STATUSBAR_H_

// .NAME vgui_qt_statusbar - hmm untested ?
// .HEADER vxl package
// .LIBRARY vgui-qt
// .INCLUDE vgui/impl/qt/vgui_qt_statusbar.h
// .FILE vgui_qt_statusbar.cxx

// .SECTION Author
// Joris Schouteden

// .SECTION Modifications:
// 24.03.2000 JS  Initial Version, adapted from vgui_gtk_statusbar

#include <vcl_string.h>
#include <vcl_iosfwd.h>

#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

#include <qstatusbar.h>

//: QT implementation of vgui_statusbar.
class vgui_qt_statusbar :
   public QStatusBar,
   public vgui_statusbar
{
   Q_OBJECT
public:
   vgui_qt_statusbar();
  ~vgui_qt_statusbar();

   int write(const char* text, int n);
   int write(const char* text);

   vcl_string linebuffer;
   vgui_statusbuf* statusbuf;
   vcl_ostream out;
};

#endif // VGUI_QT_STATUSBAR_H_
