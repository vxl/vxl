// This is oxl/ogui/impl/Fl/vgui_Fl_adaptor.h
#ifndef vgui_Fl_adaptor_h_
#define vgui_Fl_adaptor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief FLTK implementation of vgui_adaptor.
//
// \author
//              Philip C. Pritchett, 11 Sep 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <FL/Fl_Gl_Window.H>
#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>

class Fl_Menu_Button;

//: FLTK implementation of vgui_adaptor.
class vgui_Fl_adaptor : public Fl_Gl_Window, public vgui_adaptor, public vgui_adaptor_mixin
{
 public:
  typedef vgui_adaptor_mixin mixin;

  vgui_Fl_adaptor();
  vgui_Fl_adaptor(int x, int y, int w, int h, const char* L=0);
  ~vgui_Fl_adaptor();

  // vgui_adaptor methods
  void swap_buffers();
  void make_current();
  void post_redraw();
  void post_overlay_redraw();
  unsigned get_width() const {return mixin::width;}
  unsigned get_height() const {return mixin::height;}

  void bind_popups(vgui_modifier, vgui_button);
  void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
    { m = mixin::popup_modifier; b = mixin::popup_button; }

  // fltk methods
  void draw();
  void draw_overlay();
  int handle(int);

 protected:
  bool is_drawn;
  Fl_Menu_Button* popup;
 private:
  vgui_menu last_popup; // <--- ask fsm about this.
};

#endif // vgui_Fl_adaptor_h_
