// This is oxl/vgui/impl/Xm/vgui_Xm_adaptor.h
#ifndef vgui_Xm_adaptor_h_
#define vgui_Xm_adaptor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Undocumented class FIXME
//
// \author
//              Philip C. Pritchett, 11 Sep 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>
#include <vgui/vgui_glx.h>
#include <X11/Intrinsic.h>
// It seems that the location of GLwMDrawA.h has moved in recent
// versions of Mesa. If we need to support both, we should put in a
// mechanism to figure out where it is.
//#include <X11/GLw/GLwMDrawA.h>
#include <GL/GLwMDrawA.h>

class Fl_Menu_Button;

class vgui_Xm_adaptor : public vgui_adaptor, public vgui_adaptor_mixin
{
 public:
  typedef vgui_adaptor_mixin mixin;

  vgui_Xm_adaptor();
  vgui_Xm_adaptor(int x, int y, int w, int h, const char* title=0);
  ~vgui_Xm_adaptor();

  // vgui_adaptor methods
  void swap_buffers();
  void make_current();

  void post_redraw();
  void post_overlay_redraw();

  unsigned get_width() const {return mixin::width;}
  unsigned get_height() const {return mixin::height;}
  void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
    { m = mixin::popup_modifier; b = mixin::popup_button; }

  // motif stuff
  void init();
  void draw();

  Widget widget;
  GLXContext context;
  Widget xpopup;

  void setup_widget(Widget *frame, XVisualInfo *visinfo);
  void setup_context(Display *dpy, XVisualInfo *visinfo);
  void create_popup_menu(Widget form, const vgui_menu& menu);


  static void input_cb(Widget w, XtPointer client_data, XtPointer call);
  static void expose_cb(Widget w, XtPointer client_data, XtPointer call);
  static void resize_cb(Widget w, XtPointer client_data, XtPointer call);

 protected:
  bool is_drawn;
 private:
  static vgui_menu last_popup;
};

#endif // vgui_Xm_adaptor_h_
