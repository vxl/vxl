#include "vgui_qt_adaptor.h"
#include "vgui_qt_menu.h"

#include <vgui/vgui_popup_params.h>

#include <qgl.h>

//--------------------------------------------------------------------------------
void vgui_qt_adaptor::paintGL()
{
   make_current();
   if (this->doubleBuffer()) glDrawBuffer(GL_BACK);
   else glDrawBuffer(GL_FRONT);
   dispatch_to_tableau(vgui_DRAW);
// FIXME
//   swap_buffers();
}


//--------------------------------------------------------------------------------
void vgui_qt_adaptor::paintOverlayGL()
{
   make_current();
   if (this->doubleBuffer()) glDrawBuffer(GL_BACK);
   else glDrawBuffer(GL_FRONT);
   dispatch_to_tableau(vgui_DRAW_OVERLAY);
// FIXME
//   swap_buffers();
}


//--------------------------------------------------------------------------------
void vgui_qt_adaptor::resizeGL(int w, int h)
{
   make_current();

   vgui_adaptor_mixin::width  = QGLWidget::width();
   vgui_adaptor_mixin::height = QGLWidget::height();

   dispatch_to_tableau(vgui_RESHAPE);
}


//--------------------------------------------------------------------------------
void vgui_qt_adaptor::mouseMoveEvent   (QMouseEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_MOTION;
   dispatch_to_tableau(ev);
}


//--------------------------------------------------------------------------------
void vgui_qt_adaptor::mousePressEvent  (QMouseEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_BUTTON_DOWN;

   // popup
   if (ev.button   == this->popup_button &&
       ev.modifier == this->popup_modifier)
   {
      vgui_popup_params params;
      params.x = ev.wx;
      params.y = ev.wy;
      QPopupMenu* pm = new vgui_qt_menu(this->get_total_popup(params));

      pm->popup(QWidget::mapToGlobal(QPoint(e->x(), e->y())));

      return;
   }

   dispatch_to_tableau(ev);
}


//--------------------------------------------------------------------------------
void vgui_qt_adaptor::mouseReleaseEvent(QMouseEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_BUTTON_UP;
   dispatch_to_tableau(ev);
}

//--------------------------------------------------------------------------------
void vgui_qt_adaptor::keyPressEvent   (QKeyEvent* e) {}
void vgui_qt_adaptor::keyReleaseEvent (QKeyEvent* e) {}
void vgui_qt_adaptor::focusInEvent  (QFocusEvent * e) {}
void vgui_qt_adaptor::focusOutEvent (QFocusEvent * e) {}


//--------------------------------------------------------------------------------
vgui_event vgui_qt_adaptor::translate(QMouseEvent* e)
{
   vgui_event ev;
   ev.button = vgui_BUTTON_NULL;
   if (e->button() & LeftButton) ev.button = vgui_LEFT;
   if (e->button() & RightButton) ev.button = vgui_RIGHT;
   if (e->button() & MidButton) ev.button = vgui_MIDDLE;
   ev.modifier = vgui_MODIFIER_NULL;
   if (e->state() & ControlButton) ev.modifier = vgui_CTRL;
   if (e->state() & AltButton) ev.modifier = vgui_ALT;
   if (e->state() & ShiftButton) ev.modifier = vgui_SHIFT;
   ev.wx = e->x();
   ev.wy = QGLWidget::height() - e->y();
   return ev;
}
