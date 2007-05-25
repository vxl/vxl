// This is core/vgui/impl/qt4/vgui_qt_window.h
#ifndef vgui_qt_window_h_
#define vgui_qt_window_h_
//:
// \file
// \brief qt windows
// \author Joris Schouteden, ESAT, K.U.Leuven
//
// \verbatim
//  Modifications
//   24.03.2000 JS  Initial Version, adapted from vgui_gtk_window
//   14.11.2005 Chanop Silpa-Anan  adapted to QT 3.3.5 for X11/Mac
//   02.05.2007 Christoph_John@gmx.de ported to QT 4.2.2
//   23.05.2007 Matt Leotta  converted to QT3 compatibility functions to native QT4
// \endverbatim

#include <vgui/vgui_window.h>
#include <vgui/vgui_menu.h>

#include "vgui_qt_statusbar.h"
#include "vgui_qt_adaptor.h"

#include <qmainwindow.h>
#include <qobject.h>

class vgui_gtk_adaptor;

//: QT implementation of vgui_window.
class vgui_qt_window :
   public QMainWindow,
   public vgui_window
{
 public:
   vgui_qt_window(int w, int h, const vgui_menu& menu, const char* title);
   vgui_qt_window(int w, int h, const char* title);
  ~vgui_qt_window() { };

   void set_menubar(const vgui_menu &menu);

   vgui_statusbar* get_statusbar() { return &statusbar; }
   vgui_adaptor* get_adaptor() { return adaptor; };

   void show() { QMainWindow::show(); };
   void hide() { QMainWindow::hide(); };

 private:
   void setup_widget(int w, int h, const char* title);

   vgui_qt_adaptor*  adaptor ;
   vgui_qt_statusbar statusbar;

   bool use_menubar;
   bool use_statusbar;
};

#endif // vgui_qt_window_h_
