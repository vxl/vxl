// This is core/vgui/internals/vgui_overlay_helper.h
#ifndef vgui_overlay_helper_h_
#define vgui_overlay_helper_h_
//:
// \file
// \author fsm
// \brief  Helper class to emulate overlays using an auxiliary buffer.

class vgui_event;
class vgui_adaptor;

//: A helper class which emulates overlays using an auxiliary buffer.
//  The auxiliary buffer may be an OpenGL "aux buffer", the OpenGL
//  back buffer, a Mesa pixmap or some other resource.
//
//  To use it, the adaptor must relinquish control of event dispatch
//  to the vgui_overlay_helper using the dispatch method and also
//  call post_overlay_redraw() when an overlay redraw is posted on
//  the adaptor.
struct vgui_overlay_helper
{
  vgui_overlay_helper(vgui_adaptor *);
  ~vgui_overlay_helper();

  // client methods
  void post_overlay_redraw() { overlay_redraw_posted = true; }
  bool dispatch(vgui_event const &e);

 private:
  vgui_adaptor *adaptor;
  bool aux_buffer_is_back_buffer;
  bool last_draw_was_overlay;
  bool overlay_redraw_posted;

  // these helpers implement dispatch()
  bool dispatch_draw(vgui_event const &);
  bool dispatch_other(vgui_event const &);
  bool dispatch_overlay_draw(vgui_event const &);
};

#endif // vgui_overlay_helper_h_
