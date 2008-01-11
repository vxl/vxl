#ifndef vgui_qt_adaptor_h_
#define vgui_qt_adaptor_h_

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
//   02.05.2007 Christoph_John@gmx.de ported to QT 4.2.2
//   23.05.2007 Matt Leotta  converted to QT3 compatibility functions to native QT4
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_map.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>
#include <vgui/internals/vgui_overlay_helper.h>

#include <QGLWidget>
#include <QEvent>
#include <QObject>
#include <QTimer>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>

class vgui_qt_internal_timer;

//: OpenGL canvas in QT as a VGUI adaptor
class vgui_qt_adaptor :
   public QGLWidget,
   public vgui_adaptor,
   public vgui_adaptor_mixin
{
   Q_OBJECT
 public:
   vgui_qt_adaptor(QWidget* parent=0);
   ~vgui_qt_adaptor();

   void swap_buffers()
   {
     make_current ();
     if (doubleBuffer()) swapBuffers();
   }
   void make_current() { makeCurrent(); }
   void post_redraw()  { updateGL(); }
   void post_overlay_redraw();
   void post_idle_request();
   
   void post_timer(float, int );
   void kill_timer(int);

   unsigned int get_width()  const { return QGLWidget::width(); }
   unsigned int get_height() const { return QGLWidget::height(); }
   void bind_popups(vgui_modifier m, vgui_button b)
   {
      vgui_adaptor_mixin::popup_modifier = m;
      vgui_adaptor_mixin::popup_button = b;
   }
   void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
   {
      m = vgui_adaptor_mixin::popup_modifier;
      b = vgui_adaptor_mixin::popup_button;
   }

   void set_default_popup(vgui_menu) {}
   vgui_menu get_popup() { return vgui_menu(); }

 public:
   void paintGL();
   void paintOverlayGL();
   void resizeGL(int w, int h);

   void mouseMoveEvent   (QMouseEvent* e);
   void mousePressEvent  (QMouseEvent* e);
   void mouseReleaseEvent(QMouseEvent* e);
   void keyPressEvent    (QKeyEvent*   e);
   void keyReleaseEvent  (QKeyEvent*   e);
   void wheelEvent       (QWheelEvent* e);

   vgui_event translate(QMouseEvent* e);
   vgui_event translate(QKeyEvent* e);
   vgui_event translate(QWheelEvent* e);

   static vgui_key translate(Qt::Key k);
   static vgui_modifier translate(Qt::KeyboardModifiers m);

   static Qt::Key translate(vgui_key k);
   static Qt::KeyboardModifiers translate(vgui_modifier m);

 private:
   // pointer to overlay emulation data
   vgui_overlay_helper *ovl_helper;
   bool dispatch_to_tableau (const vgui_event &event);
   bool use_overlay_helper;
   bool idle_request_posted_;
   QTimer* idle_timer_;
   
   // map of timers currently in use
   vcl_map<int, vgui_qt_internal_timer*>  timers_;

 private slots:
   void idle_slot();

 protected:
   void windowActivationChange (bool oldActive);
   
   friend class vgui_qt_internal_timer;
};



//: A helper QObject to trigger numbered timer events 
class vgui_qt_internal_timer: public QObject
{
   Q_OBJECT
 public:
   
   vgui_qt_adaptor* adaptor;
   int id;
   
   vgui_qt_internal_timer() : adaptor(0), id(0) {}
   vgui_qt_internal_timer(vgui_qt_adaptor* a, int i) : adaptor(a), id(i) {}
   
 public slots:
   void activate(); 
};

#endif // vgui_qt_adaptor_h_
