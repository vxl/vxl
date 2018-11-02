/*
  fsm
*/
#include <cstdlib>
#include <cstring>
#include <csetjmp>
#include <iostream>
#include <list>
#include <utility>
#include "vgui_glut_impl.h"
#include "vgui_glut_window.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgui/vgui_glut.h>

//--------------------------------------------------------------------------------

vgui_glut_impl* vgui_glut_impl::instance()
{
  static vgui_glut_impl instance_;
  return &instance_;
}

//--------------------------------------------------------------------------------

int vgui_glut_impl::count = 0;

vgui_glut_impl::vgui_glut_impl()
{
  ++count;
  assert(count == 1);
}

vgui_glut_impl::~vgui_glut_impl()
{
  --count;
  assert(count==0);
}

//--------------------------------------------------------------------------------

// See vgui/vgui_text_put.cxx
extern bool glut_was_initialized;

void vgui_glut_impl::init(int &argc, char **argv)
{
#ifdef DEBUG
  std::cerr << __FILE__ " init() :\n";
  for (unsigned i=0; i<argc; ++i)
    std::cerr << i << ' ' << (void*)argv[i] << ' ' << argv[i] << std::endl;
#endif
  if( ! glut_was_initialized ) {
    glut_was_initialized = true;
    glutInit( &argc, argv );
  }
}

std::string vgui_glut_impl::name() const
{
  return "glut";
}

vgui_window *vgui_glut_impl::produce_window(int width, int height,
                                            vgui_menu const &menubar,
                                            char const *title)
{
  vgui_glut_window *win = new vgui_glut_window(title, width, height);
  win->menubar = menubar;
  return win;
}

vgui_window *vgui_glut_impl::produce_window(int width, int height,
                                            char const *title)
{
  return new vgui_glut_window(title, width, height);
}

//----------------------------------------------------------------------

#include <vgui/vgui_macro.h>

// Use setjmp()/longjmp() to circumvent GLUT event loop restrictions.
//
// It is essential never to longjmp() across C++ destructors. It
// happens to be safe to longjmp() out of glutMainLoop() from the
// GLUT idle callback, by inspection of the GLUT sources.
//
// main()
//  ...
//   vgui::run()
//    ...
//    vgui_glut_impl::run()
//    ...
//     internal_run_till_idle();
//     goto next_statement;// this is what setjmp() effectively does
//  longjmp_target: // When the idle callback is called, it will longjmp to here.
//     goto idle;
//  next_statement:
//     glutMainLoop()
//     std::exit(); // [gets here only on close of window.]
//
//  idle:
//       [user code]
//       ...

static int const   internal_label = 1234;
static std::jmp_buf internal_buf;

// This function is the idle callback used
// to longjmp() out of the GLUT event loop.
static
void internal_longjmp_idler()
{
  std::longjmp(internal_buf, internal_label);
  assert(false);
}

// This function lets the GLUT event loop run till it becomes
// idle and then returns to the caller. It is intended to be
// re-entrant, hence the saving and restoring of the jmp_buf.
static
void internal_run_till_idle()
{
  // save the current jmp_buf;
  std::jmp_buf saved_buf;
  std::memcpy(&saved_buf, &internal_buf, sizeof internal_buf);

  // record current state/accept control after longjmp().
  int t = setjmp(internal_buf);

/*longjmp_target:*/
  // if we got back control after a longjmp(), restore
  // the previous jmp_buf and return to the caller now.
  if (t != 0) {
    assert(t == internal_label);
    std::memcpy(&internal_buf, &saved_buf, sizeof internal_buf);
    return;
  }

/*next_statement:*/
  // set idle function.
  glutIdleFunc(internal_longjmp_idler);

  // call GLUT event loop. when the event queue becomes empty, the idle callback
  // will be called and that then longjmp()s back.
  glutMainLoop();

  // If we get here, it means glutMainLoop()
  // returned, which it should never do.
  vgui_macro_warning << "internal error in internal_run_till_idle_wrapped()\n"
                     << "please report to fsm\n";
  std::abort();
}

//--------------------------------------------------------------------------------

#include <vgui/vgui_command.h>
#include "vgui_glut_adaptor.h"

static
std::list<std::pair<void *, void *> > vgui_glut_impl_command_queue;

void vgui_glut_impl_queue_command(vgui_glut_adaptor *a, vgui_command *c)
{
  c->ref(); // matched by unref() in process_command_queue();
  vgui_glut_impl_command_queue.push_back(std::pair<void *, void *>(a, c));
}

static
void vgui_glut_impl_process_command_queue()
{
  while (! vgui_glut_impl_command_queue.empty()) {
    // remove from front of queue.
    std::pair<void *, void *> p = vgui_glut_impl_command_queue.front();
    vgui_glut_impl_command_queue.pop_front();

    // a bit of casting.
    vgui_glut_adaptor *a = static_cast<vgui_glut_adaptor *>(p.first );
    vgui_command      *c = static_cast<vgui_command *>(p.second);

    // switch to the relevant GL context.
    int old_win = glutGetWindow();
    if (old_win != a->get_id())
      glutSetWindow(a->get_id());

    // execute the command.
#ifdef DEBUG
    std::cerr << "cmnd = " << (void*)vgui_glut_impl_adaptor_menu_command << std::endl;
#endif
    c->execute();
#ifdef DEBUG
    std::cerr << "returned successfully\n";
#endif

    // this matches ref() in vgui_glut_impl_queue_command()
    c->unref();

    // switch back to the old GL context.
    if (old_win != 0 && old_win != a->get_id())
      glutSetWindow(old_win);
  }
}

// When set, this flag indicates that the event
// loop should be terminated in the near future.
static bool internal_quit_flag = false;

void vgui_glut_impl::run()
{
  internal_quit_flag = false;
  while (! internal_quit_flag) {
    internal_run_till_idle();
    vgui_glut_impl_process_command_queue();
  }
  vgui_macro_warning << "end of vgui_glut_impl event loop\n";
}

// This is (erroneously) called from vgui_glut_impl_adaptor::post_destroy().
void vgui_glut_impl_quit()
{
  internal_quit_flag = true;
}

void vgui_glut_impl::quit()
{
  internal_quit_flag = true;
}

// This is actually run-a-few-events, sorry...
void vgui_glut_impl::run_one_event()
{
  internal_run_till_idle();
  vgui_glut_impl_process_command_queue();
}

void vgui_glut_impl::run_till_idle()
{
  internal_run_till_idle();
  vgui_glut_impl_process_command_queue();
}

void vgui_glut_impl::flush()
{
  glFlush();
  run_till_idle();
}

//--------------------------------------------------------------------------------
