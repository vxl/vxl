// This is core/vgui/impl/qt/vgui_qt_statusbar.h
#ifndef VGUI_QT_STATUSBAR_H_
#define VGUI_QT_STATUSBAR_H_
//:
// \file
// \brief Contains class vgui_qt_statusbar
// \author Joris Schouteden, ESAT, K.U.Leuven
//
// \verbatim
//  Modifications
//   24.03.2000 JS  Initial Version, adapted from vgui_gtk_statusbar
//   14.11.2005 Chanop Silpa-Anan  adapted to QT 3.3.5 for X11/Mac
//   02.05.2007 Christoph_John@gmx.de ported to QT 4.2.2
// \endverbatim

#include <vcl_string.h>
#include <vcl_iosfwd.h>

#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

#include <qstatusbar.h>
#include <q3mainwindow.h>

//: QT implementation of vgui_statusbar.
class vgui_qt_statusbar :
   public vgui_statusbar
{
 public:
   vgui_qt_statusbar(Q3MainWindow *parent);
  ~vgui_qt_statusbar();

   int write(const char* text, int n);
   int write(const char* text);

   vcl_string linebuffer;
   vgui_statusbuf* statusbuf;
   vcl_ostream out;

 private:
   Q3MainWindow *parent_;
};

#endif // VGUI_QT_STATUSBAR_H_
