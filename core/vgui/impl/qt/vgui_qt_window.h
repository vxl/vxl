#ifndef VGUI_QT_WINDOW_H_
#define VGUI_QT_WINDOW_H_

// .NAME vgui_qt_window - qt vensters
// .HEADER vxl package
// .LIBRARY vgui-qt
// .INCLUDE vgui/impl/qt/vgui_qt_window.h
// .FILE vgui_qt_window.cxx

// .SECTION Author
// Joris Schouteden

// .SECTION Modifications:
// 24.03.2000 JS  Initial Version, adapted from vgui_gtk_window

#include <vgui/vgui_window.h>
#include <vgui/vgui_menu.h>

#include "vgui_qt_statusbar.h"
#include "vgui_qt_adaptor.h"

#include <qmainwindow.h>

class vgui_gtk_adaptor;

//: QT implemenation of vgui_window.
class vgui_qt_window :
   public QMainWindow,
   public vgui_window
{
public:

   vgui_qt_window(int w, int h, const vgui_menu& menu, const char* title);
   vgui_qt_window(int w, int h, const char* title);
  ~vgui_qt_window() { };

   void set_menubar(const vgui_menu &menu);
   void set_statusbar(bool) { };

   void set_adaptor(vgui_adaptor*);
   vgui_adaptor* get_adaptor() { return adaptor; };

   void show() { QMainWindow::show(); };
   void hide() { QMainWindow::hide(); };

   vgui_qt_adaptor*  adaptor ;
   vgui_qt_statusbar statusbar;

   bool use_menubar;
   bool use_statusbar;

private:
   void setup_widget(int w, int h, const char* title);
};

#endif // VGUI_QT_WINDOW_H_
