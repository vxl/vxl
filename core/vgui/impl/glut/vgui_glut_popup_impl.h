// This is core/vgui/impl/glut/vgui_glut_popup_impl.h
#ifndef vgui_glut_popup_impl_h_
#define vgui_glut_popup_impl_h_
// .NAME vgui_glut_popup_impl
// .LIBRARY vgui-glut
// .INCLUDE vgui/impl/glut/vgui_glut_popup_impl.h
// .FILE vgui_glut_popup_impl.cxx
// @author fsm

#include <vgui/vgui_menu.h>

struct vgui_glut_popup_impl
{
  int menu_id;  // GLUT menu id of this popup_impl.

  vgui_glut_popup_impl();
  ~vgui_glut_popup_impl();

  // clear the popup_impl. why is this public?
  void clear();

  // build a glut menu from a vgui_menu.
  // does not affect which menu is current.
  void build(vgui_menu const &m);

 private:
  static void command_func(int value);

  std::vector<void*> subs; // list of pointers to popup_impls allocated by this popup_impl.
  vgui_menu tmp_menu;     // see build_internal().

  void build_internal(vgui_menu const &m);
};

#endif // vgui_glut_popup_impl_h_
