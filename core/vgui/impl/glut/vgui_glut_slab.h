// This is oxl/vgui/impl/glut/vgui_glut_slab.h
#ifndef vgui_glut_slab_h_
#define vgui_glut_slab_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_list.h>
#include <vgui/vgui_glut.h>
#include <vgui/vgui_slab.h>
#include <vgui/vgui_event.h>

struct vgui_glut_slab : public vgui_slab
{
  int window;

  vgui_glut_slab(int w);
  ~vgui_glut_slab();

  void post_redraw();
  void post_overlay_redraw();

  int width () const;
  int height() const;

  void swap_buffers();
  void make_current();

  //void overlay_enter ();
  //bool overlay_active() const;
  //void overlay_leave ();

  //void draw_begin();
  //void draw_end  ();

  bool queue_empty() const;
  bool queue_peek (vgui_event *, int);
  bool queue_pop  ();
  bool queue_push (vgui_event const &);

 private:
  int old_window;
  void enter() const;
  void leave() const;

  vcl_list<vgui_event> queue;
  void glut_insert(vgui_event const &);

#define macro(name, proto) \
  void name proto; \
  static void name##_callback proto

  macro(        display, ());
  macro(overlay_display, ());
  macro(        reshape, (int width, int height));
  macro(       keyboard, (unsigned char, int, int));
  macro(          mouse, (int button, int state, int x, int y));
  macro(         motion, (int x, int y));
  macro( passive_motion, (int x, int y));
  macro(          entry, (int));
  macro(     visibility, (int state));
  macro(           idle, ());
  macro(          timer, (int));
  macro(        special, (int, int, int));
  macro(    keyboard_up, (unsigned char, int, int));
  macro(     special_up, (int, int, int));
  macro(     menustatus, (int, int, int));
#undef macro

  void register_static_callbacks() const;
};

#endif // vgui_glut_slab_h_
