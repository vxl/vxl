#include "vgui_qt_adaptor.h"
#include "vgui_qt_menu.h"

#include <vgui/vgui_popup_params.h>

#include <qgl.h>
#include <vcl_iostream.h>

vgui_qt_adaptor::vgui_qt_adaptor(QWidget* parent, const char* name)
   : QGLWidget(QGLFormat(DoubleBuffer|DepthBuffer|Rgba|AlphaChannel|
                         AccumBuffer|StencilBuffer|NoStereoBuffers|
                         DirectRendering|HasOverlay), parent, name),
//    : QGLWidget(parent, name)
     ovl_helper(0),
     use_overlay_helper(true),
     idle_request_posted_(false)
{
   this->setMouseTracking(true);
   this->setFocusPolicy(QWidget::StrongFocus);
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
   idle_timer_ = new QTimer (this, "vgui_qt_adaptor_idle_timer");
   idle_timer_-> start (0);
   connect (idle_timer_, SIGNAL(timeout()), this, SLOT(idle_slot()));
}


//--------------------------------------------------------------------------------
vgui_qt_adaptor::~vgui_qt_adaptor()
{
   if (ovl_helper)
     delete ovl_helper;
   ovl_helper = 0;
}


//--------------------------------------------------------------------------------
bool
vgui_qt_adaptor::dispatch_to_tableau (const vgui_event &event)
{
  if (ovl_helper)
    return ovl_helper->dispatch(event);
  else
    return vgui_adaptor::dispatch_to_tableau(event);
}

//--------------------------------------------------------------------------------
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


//--------------------------------------------------------------------------------
void vgui_qt_adaptor::post_idle_request()
{
   idle_request_posted_ = true;
}

//--------------------------------------------------------------------------------
void vgui_qt_adaptor::paintOverlayGL()
{
   if (this->doubleBuffer())
     glDrawBuffer(GL_BACK);
   else
     glDrawBuffer(GL_FRONT);
   dispatch_to_tableau(vgui_DRAW_OVERLAY);
   swap_buffers ();
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

void vgui_qt_adaptor::focusInEvent  (QFocusEvent * e) {}
void vgui_qt_adaptor::focusOutEvent (QFocusEvent * e) {}


//--------------------------------------------------------------------------------
vgui_event vgui_qt_adaptor::translate(QMouseEvent* e)
{
   vgui_event ev;
   ev. button = vgui_BUTTON_NULL;
   if (e-> button () & LeftButton) ev. button = vgui_LEFT;
   if (e-> button () & RightButton) ev. button = vgui_RIGHT;
   if (e-> button () & MidButton) ev. button = vgui_MIDDLE;

   int state = vgui_MODIFIER_NULL;
   if (e-> state () & ControlButton) state |= vgui_CTRL;
   if (e-> state () & AltButton) state |= vgui_ALT;
   if (e-> state () & ShiftButton) state |= vgui_SHIFT;
   ev. modifier = vgui_modifier (state);
   ev. wx = e-> x ();
   ev. wy = QGLWidget::height () - e-> y ();
   return ev;
}

vgui_event vgui_qt_adaptor::translate(QKeyEvent* e)
{
   vgui_event ev;
   int state = vgui_MODIFIER_NULL;
   if (e-> state () & ControlButton) state |= vgui_CTRL;
   if (e-> state () & AltButton) state |= vgui_ALT;
   if (e-> state () & ShiftButton) state |= vgui_SHIFT;
   ev. modifier = vgui_modifier (state);

   vgui_key key = vgui_KEY_NULL;

   switch (e-> key())
   {
   case Qt::Key_Prior:
     key = vgui_PAGE_UP;
     break;
   case Qt::Key_Next:
     key = vgui_PAGE_DOWN;
     break;
   case Qt::Key_Home:
     key = vgui_HOME;
     break;
   case Qt::Key_End:
     key = vgui_END;
     break;
   case Qt::Key_Delete:
     key = vgui_DELETE;
     break;
   case Qt::Key_Insert:
     key = vgui_INSERT;
     break;
   case Qt::Key_Up:
     key = vgui_CURSOR_UP;
     break;
   case Qt::Key_Down:
     key = vgui_CURSOR_DOWN;
     break;
   case Qt::Key_Left:
     key = vgui_CURSOR_LEFT;
     break;
   case Qt::Key_Right:
     key = vgui_CURSOR_RIGHT;
     break;
   default:
     key = vgui_key (e-> ascii ());
     break;
   }

   ev. set_key (key);
   ev. ascii_char = key;

   return ev;
}
