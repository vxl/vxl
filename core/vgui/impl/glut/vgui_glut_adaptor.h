// This is core/vgui/impl/glut/vgui_glut_adaptor.h
#ifndef vgui_glut_adaptor_h_
#define vgui_glut_adaptor_h_
//:
// \file
// \brief  The GLUT (OpenGL Utility Toolkit) implementation of vgui_adaptor.
// \author fsm
//
// \verbatim
//  Modifications
//   17-Sep-2002 K.Y.McGaul - Converted to doxygen style documentation.
// \endverbatim

#include <vcl_vector.h>
#include <vgui/vgui_adaptor.h>
struct vgui_command;
struct vgui_overlay_helper;
struct vgui_glut_window;

//: The GLUT (OpenGL Utility Toolkit) implementation of vgui_adaptor.
struct vgui_glut_adaptor : vgui_adaptor
{
  vgui_glut_adaptor( vgui_glut_window *win_, int id_ );
  ~vgui_glut_adaptor();

  int get_id() const { return id; }

  // implement virtual vgui_adaptor interface :
  vgui_window *get_window() const;
  void post_redraw();
  void post_overlay_redraw();
  void post_timer(float, int);
  void post_destroy();
  void swap_buffers();
  void make_current();
  void bind_popups(vgui_modifier ,vgui_button );
  unsigned get_width() const;
  unsigned get_height() const;

  void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
    { m = popup_modifier; b = popup_button; }

 private:
  int id;
  vgui_glut_window *win;

  vgui_modifier popup_modifier;
  vgui_button popup_button;

  // Overlay state. Before any overlay specific code may run, the overlay
  // has to be established by calling establish_overlays(). This means
  // either that GLUT creates a hardware overlay plane or that emulation
  // overlays are used (via a vgui_overlay_helper object).
  bool ovl_established;              // True if overlays have been established
  vgui_overlay_helper *ovl_helper;   // If ovl_established, this is 0 when using
  void establish_overlays();         // hardware overlays.

  // Performs conversion of event position from window coordinates to
  // viewport coordinates. Then dispatches either to the overlay_helper
  // or to dispatch_to_tableau() on the base class.
  bool glut_dispatch(vgui_event &e);

  //
  void  register_static_callbacks();

  // super and sub
  vgui_glut_adaptor *super; // 0 if none.
  vcl_vector<vgui_glut_adaptor*> sub_contexts;

  // popup things
  friend struct vgui_glut_popup_impl;
  vgui_glut_popup_impl *popup;
  void make_popup();
  static void pre_menu_hook(int );
  //static void queue_command(vgui_command *);

  // dynamic callbacks :
  virtual void         display();
  virtual void overlay_display();
  virtual void         reshape(int width,int height);
  virtual void        keyboard(unsigned char,int,int);
  virtual void           mouse(int button,int state,int x,int y);
  virtual void          motion(int x,int y);
  virtual void  passive_motion(int x,int y);
  virtual void           entry(int);
  virtual void      visibility(int state);
  //virtual void            idle();
  virtual void           timer(int);
  virtual void         special(int,int,int);
  virtual void     keyboard_up(unsigned char,int,int);
  virtual void      special_up(int,int,int);

  //
  static vcl_vector<vgui_glut_adaptor*> &all();
  static vgui_glut_adaptor *get_adaptor(int window_id);

  // static callbacks :
  static void         display_callback();
  static void overlay_display_callback();
  static void         reshape_callback(int width,int height);
  static void        keyboard_callback(unsigned char,int,int);
  static void           mouse_callback(int button,int state,int x,int y);
  static void          motion_callback(int x,int y);
  static void  passive_motion_callback(int x,int y);
  static void           entry_callback(int);
  static void      visibility_callback(int state);
  //static void            idle_callback();
  static void           timer_callback(int);
  static void         special_callback(int,int,int);
  static void     keyboard_up_callback(unsigned char,int,int);
  static void      special_up_callback(int,int,int);
  static void      menustatus_callback(int,int,int);
};

#endif // vgui_glut_adaptor_h_
