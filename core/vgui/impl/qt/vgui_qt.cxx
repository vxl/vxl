#include "vgui_qt.h"
#include "vgui_qt_window.h"
#include "vgui_qt_dialog_impl.h"

#include <qapplication.h>


//-----------------------------------------------------------------------------
vgui_qt* vgui_qt::instance()
{
  static vgui_qt* instance_ = new vgui_qt;
  return instance_;
}


//-----------------------------------------------------------------------------
void vgui_qt::init(int &argc, char **argv)
{
   new QApplication(argc, argv);
}


//-----------------------------------------------------------------------------
void vgui_qt::run()
{
   qApp->exec();
}


//-----------------------------------------------------------------------------
vgui_window* vgui_qt::produce_window(int width, int height,
                                     const vgui_menu& menubar,
                                     const char* title)
{
   return new vgui_qt_window(width, height, menubar, title);
}


//-----------------------------------------------------------------------------
vgui_window* vgui_qt::produce_window(int width, int height,
                                     const char* title)
{
   return new vgui_qt_window(width, height, title);
}


//-----------------------------------------------------------------------------
vgui_dialog_impl* vgui_qt::produce_dialog(const char* name)
{
   return new vgui_qt_dialog_impl(name);
}
