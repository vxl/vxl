#include "vgui_qt.h"
#include <vgui/vgui_gl.h>
#include "vgui_qt_window.h"
#include "vgui_qt_dialog_impl.h"

#include <QApplication>
#include <QGLFormat>


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

#if 0
   QGLFormat f;
   f = QGLFormat::defaultFormat();
   f.setDoubleBuffer(true);
   f.setDepth(true);
   f.setRgba(true);
   f.setOverlay (true);
   f.setDirectRendering(true);
   QGLFormat::setDefaultFormat(f);

   QGLFormat of = QGLFormat::defaultOverlayFormat();
   of.setDoubleBuffer(false);
   of.setDepth(false);
   of.setRgba(false);
   of.setOverlay (false);
   of.setDirectRendering(true);
   QGLFormat::setDefaultOverlayFormat(of);


   vcl_cerr << "QGLFormat::hasOpenGL () is " << QGLFormat::hasOpenGL () << vcl_endl
            << "QGLFormat::hasOpenGLOverlays () is " << QGLFormat::hasOpenGLOverlays () << vcl_endl;
#endif
}


//-----------------------------------------------------------------------------
void vgui_qt::run()
{
   qApp->exec();
}

//-----------------------------------------------------------------------------
void vgui_qt::run_one_event()
{
   qApp->processEvents(QEventLoop::AllEvents, 10);
   glFlush();
}


//-----------------------------------------------------------------------------
void vgui_qt::run_till_idle()
{
   qApp->processEvents(QEventLoop::AllEvents, 10);
   glFlush();
}

//-----------------------------------------------------------------------------
void vgui_qt::flush()
{
  glFlush();
  run_till_idle();
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
