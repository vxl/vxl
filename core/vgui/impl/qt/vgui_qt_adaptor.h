#ifndef VGUI_QT_ADAPTOR_H_
#define VGUI_QT_ADAPTOR_H_

//:
// \file
// \brief OpenGL canvas in QT as a VGUI adaptor
//
// \author
// Joris Schouteden, ESAT, K.U.Leuven
//
// \verbatim
//  Modifications
//   24.03.2000 JS  Initial Version, adapted from vgui_gtk_adaptor
//   14.11.2005 Chanop Silpa-Anan  adapted to QT 3.3.5 for X11/Mac
//   17.05.2006 Rowin Heymans      added mousewheel event handling
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>
#include <vgui/internals/vgui_overlay_helper.h>

#include <qgl.h>
#include <qevent.h>
#include <qobject.h>
#include <qtimer.h>

//: OpenGL canvas in QT as a VGUI adaptor
class vgui_qt_adaptor
  : public QGLWidget
  , public vgui_adaptor
  , public vgui_adaptor_mixin
{
  Q_OBJECT
public:
  vgui_qt_adaptor(QWidget * parent = 0, const char * name = 0);
  ~vgui_qt_adaptor();

  void
  swap_buffers()
  {
    make_current();
    if (doubleBuffer())
      swapBuffers();
  }
  void
  make_current()
  {
    makeCurrent();
  }
  void
  post_redraw()
  {
    updateGL();
  }
  void
  post_overlay_redraw();
  void
  post_idle_request();

  unsigned int
  get_width() const
  {
    return QGLWidget::width();
  }
  unsigned int
  get_height() const
  {
    return QGLWidget::height();
  }
  void
  bind_popups(vgui_modifier m, vgui_button b)
  {
    vgui_adaptor_mixin::popup_modifier = m;
    vgui_adaptor_mixin::popup_button = b;
  }
  void
  get_popup_bindings(vgui_modifier & m, vgui_button & b) const
  {
    m = vgui_adaptor_mixin::popup_modifier;
    b = vgui_adaptor_mixin::popup_button;
  }

  void
  set_default_popup(vgui_menu)
  {}
  vgui_menu
  get_popup()
  {
    return vgui_menu();
  }

public:
  void
  paintGL();
  void
  paintOverlayGL();
  void
  resizeGL(int w, int h);

  void
  mouseMoveEvent(QMouseEvent * e);
  void
  mousePressEvent(QMouseEvent * e);
  void
  mouseReleaseEvent(QMouseEvent * e);
  void
  keyPressEvent(QKeyEvent * e);
  void
  keyReleaseEvent(QKeyEvent * e);
  void
  wheelEvent(QWheelEvent * e);

  vgui_event
  translate(QMouseEvent * e);
  vgui_event
  translate(QKeyEvent * e);
  vgui_event
  translate(QWheelEvent * e);

private:
  // pointer to overlay emulation data
  vgui_overlay_helper * ovl_helper;
  bool
  dispatch_to_tableau(const vgui_event & event);
  bool use_overlay_helper;
  bool idle_request_posted_;
  QTimer * idle_timer_;

private slots:
  void
  idle_slot();

protected:
  void
  windowActivationChange(bool oldActive);
};

#endif // VGUI_QT_ADAPTOR_H_
