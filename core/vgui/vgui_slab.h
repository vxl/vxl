// This is oxl/vgui/vgui_slab.h
#ifndef vgui_slab_h_
#define vgui_slab_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME vgui_slab
// .INCLUDE vgui/vgui_slab.h
// .FILE vgui_slab.cxx
// @author fsm@robots.ox.ac.uk

#include <vgui/vgui_event.h>

struct vgui_slab
{
  vgui_slab();
  virtual ~vgui_slab();

  // -------------------- misc

  //vgui_window *window() const { return 0; }
  //vgui_toolkit *toolkit() const { return 0; }
  virtual int  post_timer(float);
  virtual void post_timer(float, int );
  virtual void post_redraw();
  virtual void post_overlay_redraw();

  // -------------------- frame buffer

  // size, in pixels
  virtual int width () const =0;
  virtual int height() const =0;

  virtual void swap_buffers() { }
  virtual void make_current() { }

  // enter/test for/leave overlay drawing state
  virtual void overlay_enter () { }
  virtual bool overlay_active() const { return false; }
  virtual void overlay_leave () { }

  // call these before and after drawing each frame.
  // toolkits which need special tricks to implement
  // acceleration or overlays should override these
  // methods as needed.
  virtual void draw_begin() { make_current(); }
  virtual void draw_end  () { swap_buffers(); }

  // -------------------- event queue

  virtual bool queue_empty() const =0;
  virtual bool queue_peek (vgui_event *, int =0) =0;
  virtual bool queue_pop  () =0;
  virtual bool queue_push (vgui_event const &) =0;
  virtual bool queue_next (vgui_event *pe) { return queue_peek(pe, 0) && queue_pop(); }
};

#endif // vgui_slab_h_
