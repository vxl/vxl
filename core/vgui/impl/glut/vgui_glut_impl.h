#ifndef vgui_glut_impl_h_
#define vgui_glut_impl_h_
// .NAME vgui_glut
// .LIBRARY vgui-glut
// .INCLUDE vgui/impl/glut/vgui_glut.h
// .FILE vgui_glut.cxx
// @author fsm

#include <vgui/vgui_toolkit.h>

struct vgui_glut_impl : vgui_toolkit
{
  static vgui_glut_impl* instance();

  vgui_glut_impl();
  ~vgui_glut_impl();

  void init(int &, char **);
  std::string name() const;

  vgui_window* produce_window(int width, int height,
                              vgui_menu const & menubar,
                              char const* title);

  vgui_window* produce_window(int width, int height,
                              char const* title);

  void run();
  void run_one_event();
  void run_till_idle();
  void quit();
  void flush();

private:
  static int count;
};

// Call this function to have a command queued for execution
// some time in the future.
struct vgui_command;
struct vgui_glut_adaptor;
void vgui_glut_impl_queue_command(vgui_glut_adaptor *, vgui_command *);

#endif // vgui_glut_impl_h_
