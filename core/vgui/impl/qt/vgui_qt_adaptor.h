#ifndef VGUI_QT_ADAPTOR_H_
#define VGUI_QT_ADAPTOR_H_

//:
// \file
// \brief OpenGL canvas in QT as a VGUI adaptor
//
// \author
// Joris Schouteden
//
// \verbatim
//  Modifications
//   24.03.2000 JS  Initial Version, adapted from vgui_gtk_adaptor
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>

#include <qgl.h>
#include <qevent.h>

//: OpenGL canvas in QT as a VGUI adaptor
class vgui_qt_adaptor :
   public QGLWidget,
   public vgui_adaptor,
   public vgui_adaptor_mixin
{
   Q_OBJECT
 public:
   vgui_qt_adaptor(QWidget* parent=0) : QGLWidget(parent) { this->setMouseTracking(true); }
   ~vgui_qt_adaptor() {}

   void      swap_buffers() { if (doubleBuffer()) swapBuffers(); }
   void      make_current() { makeCurrent(); }
   void      post_redraw()  { updateGL(); }
   void      post_overlay_redraw() { updateOverlayGL(); }

   unsigned  get_width()  const { return QGLWidget::width(); }
   unsigned  get_height() const { return QGLWidget::height(); }
   void      get_popup_bindings(vgui_modifier &m, vgui_button &b) const
   {
      m = vgui_adaptor_mixin::popup_modifier;
      b = vgui_adaptor_mixin::popup_button;
   }

   void      set_default_popup(vgui_menu) {}
   vgui_menu get_popup() { return vgui_menu(); }

 public:
   void      paintGL();
   void      paintOverlayGL();
   void      resizeGL(int w, int h);

   void      mouseMoveEvent   (QMouseEvent* e);
   void      mousePressEvent  (QMouseEvent* e);
   void      mouseReleaseEvent(QMouseEvent* e);
   void      keyPressEvent    (QKeyEvent*   e);
   void      keyReleaseEvent  (QKeyEvent*   e);
   void      focusInEvent     (QFocusEvent* e);
   void      focusOutEvent    (QFocusEvent* e);

   vgui_event translate(QMouseEvent* e);
};

#endif // VGUI_QT_ADAPTOR_H_
