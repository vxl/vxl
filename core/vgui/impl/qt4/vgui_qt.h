#ifndef vgui_qt_h_
#define vgui_qt_h_
//:
// \file
// \brief QT implementation of vgui_toolkit.
//
// vgui_qt is a QT implementation of vgui_toolkit.
// Provides functions for controlling the event loop.
//
// \author
// Joris Schouteden, ESAT, K.U.Leuven
//
// \verbatim
//  Modifications
//   24.03.2000 JS  Initial Version, adapted from vgui_gtk
//   14.11.2005 Chanop Silpa-Anan  adapted to QT 3.3.5 for X11/Mac
//   02.05.2007 Christoph_John@gmx.de ported to QT 4.2.2
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_toolkit.h>

class vgui_qt_adaptor;
class vgui_qt_window;

//: QT implementation of vgui_toolkit.
class vgui_qt : public vgui_toolkit
{
 public:
   static    vgui_qt*   instance();
   virtual   vcl_string name() const { return "qt"; }
   virtual   void       run();
   virtual   void       run_one_event();
   virtual   void       run_till_idle();
   virtual   void       flush();
   virtual   vgui_window* produce_window(int width, int height,
                                         const vgui_menu& menubar,
                                         const char* title="vgui qt window");
   virtual   vgui_window* produce_window(int width, int height,
                                         const char* title="vgui qt popup");
   virtual   vgui_dialog_impl* produce_dialog(const char* name);

 protected:
   vgui_qt() {}
   void      init(int &, char **);
};

#endif // vgui_qt_h_
