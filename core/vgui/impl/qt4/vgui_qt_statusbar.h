// This is core/vgui/impl/qt4/vgui_qt_statusbar.h
#ifndef vgui_qt_statusbar_h_
#define vgui_qt_statusbar_h_
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
//   23.05.2007 Matt Leotta  converted to QT3 compatibility functions to native QT4
// \endverbatim

#include <string>
#include <iosfwd>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

#include <QMainWindow>

//: QT implementation of vgui_statusbar.
class vgui_qt_statusbar :
   public vgui_statusbar
{
 public:
   vgui_qt_statusbar(QMainWindow *parent);
  ~vgui_qt_statusbar();

   int write(const char* text, int n);
   int write(const char* text);

   std::string linebuffer;
   vgui_statusbuf* statusbuf;
   std::ostream out;

 private:
   QMainWindow *parent_;
};

#endif // vgui_qt_statusbar_h_
