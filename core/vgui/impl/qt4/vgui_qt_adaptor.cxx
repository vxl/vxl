#include "vgui_qt_adaptor.h"
#include "vgui_qt_menu.h"

#include <vgui/vgui_popup_params.h>
#include <vcl_iostream.h>

#include <QMenu>

using namespace QGL;
using namespace Qt;

vgui_qt_adaptor::vgui_qt_adaptor(QWidget* parent)
//Note: enabling overlays with "HasOverlay" causes a seg fault on Mac OS X 10.4
   : QGLWidget(QGLFormat(DoubleBuffer|DepthBuffer|Rgba|AlphaChannel|
                         AccumBuffer|StencilBuffer|NoStereoBuffers|
                         DirectRendering), parent),
     ovl_helper(0),
     use_overlay_helper(true),
     idle_request_posted_(false)
{
   this->setMouseTracking(true);
   this->setFocusPolicy(Qt::StrongFocus);
   this->setAutoBufferSwap(false);

   // Check the requested GL format
   QGLFormat format = this-> format ();

   if (!format. doubleBuffer ())
     vcl_cerr << "vgui_qt_adaptor: got single buffer\n";

   if (!format. depth ())
     vcl_cerr << "vgui_qt_adaptor: no depth buffer\n";

   if (!format. rgba ())
     vcl_cerr << "vgui_qt_adaptor: index color\n";

   if (!format. directRendering ())
     vcl_cerr << "vgui_qt_adaptor: no direct rendering\n";

   // Set up idle time
   idle_timer_ = new QTimer (this);
   idle_timer_-> start (0);
   connect (idle_timer_, SIGNAL(timeout()), this, SLOT(idle_slot()));
}

//------------------------------------------------------------------------------
vgui_qt_adaptor::~vgui_qt_adaptor()
{
   if (ovl_helper)
     delete ovl_helper;
   ovl_helper = 0;
   dispatch_to_tableau(vgui_DESTROY);
}

//------------------------------------------------------------------------------
bool
vgui_qt_adaptor::dispatch_to_tableau (const vgui_event &event)
{
  if (ovl_helper)
    return ovl_helper->dispatch(event);
  else
    return vgui_adaptor::dispatch_to_tableau(event);
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::paintGL()
{
   if (this->doubleBuffer())
     glDrawBuffer(GL_BACK);
   else
     glDrawBuffer(GL_FRONT);
   dispatch_to_tableau(vgui_DRAW);
   swap_buffers ();
}

void vgui_qt_adaptor::post_overlay_redraw()
{
   if (use_overlay_helper)
   {
     if (!ovl_helper)
       ovl_helper = new vgui_overlay_helper(this);
     ovl_helper->post_overlay_redraw();
   }
   else
   {
     updateOverlayGL();
   }
}

void vgui_qt_adaptor::idle_slot ()
{
   if (idle_request_posted_)
     idle_request_posted_ =  dispatch_to_tableau( vgui_event( vgui_IDLE ) );
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::post_idle_request()
{
   idle_request_posted_ = true;
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::paintOverlayGL()
{
   if (this->doubleBuffer())
     glDrawBuffer(GL_BACK);
   else
     glDrawBuffer(GL_FRONT);
   dispatch_to_tableau(vgui_DRAW_OVERLAY);
   swap_buffers ();
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::resizeGL(int w, int h)
{
   make_current();
   vgui_adaptor_mixin::width  = QGLWidget::width();
   vgui_adaptor_mixin::height = QGLWidget::height();

   dispatch_to_tableau(vgui_RESHAPE);
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::mouseMoveEvent   (QMouseEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_MOTION;
   dispatch_to_tableau(ev);
}

//------------------------------------------------------------------------------
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
      QMenu* pm = new vgui_qt_menu(this->get_total_popup(params),this);

      pm->popup(QWidget::mapToGlobal(QPoint(e->x(), e->y())));

      return;
   }

   dispatch_to_tableau(ev);
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::mouseReleaseEvent(QMouseEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_BUTTON_UP;
   dispatch_to_tableau(ev);
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::keyPressEvent   (QKeyEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_KEY_PRESS;
   dispatch_to_tableau(ev);
}

void vgui_qt_adaptor::keyReleaseEvent (QKeyEvent* e)
{
   vgui_event ev = translate(e);
   ev.type = vgui_KEY_RELEASE;
   dispatch_to_tableau(ev);
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::wheelEvent      (QWheelEvent* e)
{
   vgui_event ev = translate(e);

   if (e->delta() > 0) ev.type = vgui_WHEEL_UP;
   else                ev.type = vgui_WHEEL_DOWN;

   dispatch_to_tableau(ev);
}

//------------------------------------------------------------------------------
void vgui_qt_adaptor::windowActivationChange (bool oldActive)
{
   vgui_event ev;

   if (!oldActive)
    ev.type = vgui_FOCUSGAINED;
   else
    ev.type = vgui_FOCUSLOST;
   dispatch_to_tableau(ev);
}

//------------------------------------------------------------------------------
vgui_event vgui_qt_adaptor::translate(QMouseEvent* e)
{
   vgui_event ev;
   ev. modifier = translate(e->modifiers());
   ev. button = vgui_BUTTON_NULL;
   if (e-> button () & Qt::LeftButton) ev. button = vgui_LEFT;
   if (e-> button () & Qt::RightButton) ev. button = vgui_RIGHT;
   if (e-> button () & Qt::MidButton) ev. button = vgui_MIDDLE;

   ev. wx = e-> x ();
   ev. wy = QGLWidget::height() - e-> y ();
   return ev;
}

vgui_event vgui_qt_adaptor::translate(QKeyEvent* e)
{
   vgui_event ev;
   ev. modifier = translate(e->modifiers());
   ev.set_key(translate(Qt::Key(e->key())));

   return ev;
}

vgui_event vgui_qt_adaptor::translate(QWheelEvent* e)
{
   vgui_event ev;
   ev. modifier = translate(e->modifiers());
   ev. wx = e-> x ();
   ev. wy = QGLWidget::height () - e-> y ();
   return ev;
}

vgui_key vgui_qt_adaptor::translate(Qt::Key k)
{
  switch (k)
  {
    case Qt::Key_Escape:
      return vgui_ESCAPE;
    case Qt::Key_Tab:
      return vgui_TAB;
    case Qt::Key_Return:
      return vgui_RETURN;
    case Qt::Key_Enter:
      return vgui_NEWLINE;
    case Qt::Key_F1:
      return vgui_F1;
    case Qt::Key_F2:
      return vgui_F2;
    case Qt::Key_F3:
      return vgui_F3;
    case Qt::Key_F4:
      return vgui_F4;
    case Qt::Key_F5:
      return vgui_F5;
    case Qt::Key_F6:
      return vgui_F6;
    case Qt::Key_F7:
      return vgui_F7;
    case Qt::Key_F8:
      return vgui_F8;
    case Qt::Key_F9:
      return vgui_F9;
    case Qt::Key_F10:
      return vgui_F10;
    case Qt::Key_F11:
      return vgui_F11;
    case Qt::Key_F12:
      return vgui_F12;
    case Qt::Key_Left:
      return vgui_CURSOR_LEFT;
    case Qt::Key_Up:
      return vgui_CURSOR_UP;
    case Qt::Key_Right:
      return vgui_CURSOR_RIGHT;
    case Qt::Key_Down:
      return vgui_CURSOR_DOWN;
    case Qt::Key_PageUp:
      return vgui_PAGE_UP;
    case Qt::Key_PageDown:
      return vgui_PAGE_DOWN;
    case Qt::Key_Home:
      return vgui_HOME;
    case Qt::Key_End:
      return vgui_END;
    case Qt::Key_Delete:
      return vgui_DELETE;
    case Qt::Key_Insert:
      return vgui_INSERT;
    default:
      return vgui_key (static_cast<unsigned char>(k));
  }

  return vgui_KEY_NULL;
}

vgui_modifier vgui_qt_adaptor::translate(Qt::KeyboardModifiers m)
{
  int mod = vgui_MODIFIER_NULL;
  if(m & Qt::CTRL)   mod |= vgui_CTRL;
  if(m & Qt::SHIFT)  mod |= vgui_SHIFT;
  if(m & Qt::META)   mod |= vgui_META;
  if(m & Qt::ALT)    mod |= vgui_ALT;

  return vgui_modifier(mod);
}

Qt::Key vgui_qt_adaptor::translate(vgui_key k)
{
  switch (k)
  {
    case vgui_ESCAPE:
      return Qt::Key_Escape;
    case vgui_TAB:
      return Qt::Key_Tab;
    case vgui_RETURN:
      return Qt::Key_Return;
    case vgui_NEWLINE:
      return Qt::Key_Enter;
    case vgui_F1:
      return Qt::Key_F1;
    case vgui_F2:
      return Qt::Key_F2;
    case vgui_F3:
      return Qt::Key_F3;
    case vgui_F4:
      return Qt::Key_F4;
    case vgui_F5:
      return Qt::Key_F5;
    case vgui_F6:
      return Qt::Key_F6;
    case vgui_F7:
      return Qt::Key_F7;
    case vgui_F8:
      return Qt::Key_F8;
    case vgui_F9:
      return Qt::Key_F9;
    case vgui_F10:
      return Qt::Key_F10;
    case vgui_F11:
      return Qt::Key_F11;
    case vgui_F12:
      return Qt::Key_F12;
    case vgui_CURSOR_LEFT:
      return Qt::Key_Left;
    case vgui_CURSOR_UP:
      return Qt::Key_Up;
    case vgui_CURSOR_RIGHT:
      return Qt::Key_Right;
    case vgui_CURSOR_DOWN:
      return Qt::Key_Down;
    case vgui_PAGE_UP:
      return Qt::Key_PageUp;
    case vgui_PAGE_DOWN:
      return Qt::Key_PageDown;
    case vgui_HOME:
      return Qt::Key_Home;
    case vgui_END:
      return Qt::Key_End;
    case vgui_DELETE:
      return Qt::Key_Delete;
    case vgui_INSERT:
      return Qt::Key_Insert;
    default:
      return Qt::Key(k);
  }

  return Qt::Key_unknown;
}

Qt::KeyboardModifiers vgui_qt_adaptor::translate(vgui_modifier m)
{
  Qt::KeyboardModifiers mod = Qt::NoModifier;
  if(m & vgui_CTRL)   mod |= Qt::ControlModifier;
  if(m & vgui_SHIFT)  mod |= Qt::ShiftModifier;
  if(m & vgui_META)   mod |= Qt::MetaModifier;
  if(m & vgui_ALT)    mod |= Qt::AltModifier;

  return mod;
}
