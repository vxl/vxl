/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_glut_slab.h"

#include <vcl_csetjmp.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_map.h>
#include <vcl_functional.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>

#include <vgui/vgui_macro.h>

#include "menu_hack.h"

void vgui_glut_slab::enter() const
{
  const_cast<vgui_glut_slab*>(this)->old_window = glutGetWindow();
  glutSetWindow( window );
}

void vgui_glut_slab::leave() const
{
  glutSetWindow( old_window );
}

//------------------------------------------------------------

void vgui_glut_slab::post_redraw()
{
  enter();
  glutPostRedisplay();
  leave();
}

void vgui_glut_slab::post_overlay_redraw()
{
  // FIXME
}

int vgui_glut_slab::width () const
{
  enter();
  int val = glutGet(GLenum(GLUT_WINDOW_WIDTH));
  leave();
  return val;
}

int vgui_glut_slab::height() const
{
  enter();
  int val = glutGet(GLenum(GLUT_WINDOW_HEIGHT));
  leave();
  return val;
}

void vgui_glut_slab::swap_buffers()
{
  enter();
  glutSwapBuffers();
  leave();
}

void vgui_glut_slab::make_current()
{
  glutSetWindow(window);
}

//void overlay_enter ();
//bool overlay_active() const;
//void overlay_leave ();

//void draw_begin();
//void draw_end  ();

//------------------------------------------------------------

void vgui_glut_slab::register_static_callbacks() const
{
  glutSetWindow(window);
  glutDisplayFunc(display_callback);
  //glutOverlayDisplayFunc(overlay_display_callback);
  glutReshapeFunc(reshape_callback);
  glutKeyboardFunc(keyboard_callback);
  glutMouseFunc(mouse_callback);
  glutMotionFunc(motion_callback);
  glutPassiveMotionFunc(passive_motion_callback);
  glutEntryFunc(entry_callback);
  glutVisibilityFunc(visibility_callback);
  //these two are global callbacks:
  //  glutIdleFunc(idle_callback);
  //  glutTimerFunc(10,timer_callback,314159);
  glutSpecialFunc(special_callback);
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13) && !defined(VCL_KAI) //wrong
  glutKeyboardUpFunc(keyboard_up_callback);
  glutSpecialUpFunc(special_up_callback);
#endif
  // this is also a global callback.
  glutMenuStatusFunc(menustatus_callback);
}

typedef vcl_map<int, void*, vcl_less<int> > map_t;

static map_t the_map;

vgui_glut_slab::vgui_glut_slab(int w) : window(w)
{
  the_map.insert(map_t::value_type(window, this));
  register_static_callbacks();
}

vgui_glut_slab::~vgui_glut_slab()
{
  map_t::iterator i = the_map.find(window);
  if (i != the_map.end())
    the_map.erase(i);
}

static vcl_jmp_buf biffer;

static void my_longjmp_idler()
{
  vcl_longjmp(biffer, 1234);
}

// this runs the glut event loop for a short
// while, in fact till it becomes idle.
static void service_glut_events()
{
  if (setjmp(biffer) == 1234)
    return;
  glutIdleFunc(my_longjmp_idler);
  glutMainLoop();
}

bool vgui_glut_slab::queue_empty() const
{
  service_glut_events();
  return queue.empty();
};

bool vgui_glut_slab::queue_peek(vgui_event *e, int pos)
{
  // Is this serious (linux-egcs with BUILD=noshared)?
  //vgui_glut_slab.cxx: In method `bool vgui_glut_slab::queue_peek(class vgui_event *, int)':
  //vgui_glut_slab.cxx:144: warning: argument `int pos' might be clobbered by `longjmp' or `vfork'
  service_glut_events();
  if (pos < 0 || pos > int(queue.size()))
    return false;
  else {
    for (vcl_list<vgui_event>::iterator i=queue.begin(); i!=queue.end(); ++i)
      if (pos == 0) {
        *e = *i;
        return true;
      }
      else
        --pos;
  }
  return false;
}

bool vgui_glut_slab::queue_pop()
{
  service_glut_events();
  if (queue.empty())
    return false;
  else {
    queue.pop_front();
    return true;
  }
}

bool vgui_glut_slab::queue_push(vgui_event const &e)
{
  service_glut_events();
  queue.push_back(e);
  return true;
}

// -------------------- glut callbacks

static vgui_glut_slab *get_slab()
{
  int win = glutGetWindow();
  map_t::iterator i = the_map.find(win);
  if (i != the_map.end())
    return (vgui_glut_slab*) (*i).second;
  else
    return 0;
}

#define implement_static_callback(name, proto, args) \
void vgui_glut_slab::name##_callback proto \
{ \
 vgui_glut_slab *v = get_slab(); \
 if (v) \
   v->name args; \
 else \
   vcl_abort(); \
}
implement_static_callback(display,(),());
implement_static_callback(overlay_display,(),());
implement_static_callback(reshape,(int width, int height),(width, height));
implement_static_callback(keyboard,(unsigned char key, int x, int y),(key, x, y));
implement_static_callback(keyboard_up,(unsigned char key, int x, int y),(key, x, y));
implement_static_callback(mouse,(int button, int state, int x, int y),(button, state, x, y));
implement_static_callback(visibility,(int state),(state));
implement_static_callback(idle,(),());
implement_static_callback(motion,(int x, int y),(x, y));
implement_static_callback(entry,(int state),(state));
implement_static_callback(passive_motion,(int x, int y),(x, y));
implement_static_callback(special,(int key, int x, int y),(key, x, y));
implement_static_callback(special_up,(int key, int x, int y),(key, x, y));

void vgui_glut_slab::menustatus_callback(int key, int x, int y)
{
}

void vgui_glut_slab::glut_insert(vgui_event const &e)
{
  queue.push_back(e);
  queue.back().wy = height()-1 - e.wy;
}

void vgui_glut_slab::display()
{
  if (glutLayerGet(GLenum(GLUT_LAYER_IN_USE)) != GLUT_NORMAL)
    vgui_macro_warning << "*** current layer is overlay" << vcl_endl;

  // normal draw.
  vgui_event e(vgui_DRAW);
  glut_insert(e);
}

void vgui_glut_slab::overlay_display()
{
  if (glutLayerGet(GLenum(GLUT_LAYER_IN_USE)) != GLUT_OVERLAY)
    vgui_macro_warning << "*** current layer is normal" << vcl_endl;

  {
    GLint isdouble=0;
    glGetIntegerv(GL_DOUBLEBUFFER, &isdouble);
    if (isdouble) // looks suspicious.....
      vgui_macro_warning << "overlay plane is double buffered" << vcl_endl;
  }

  // overlay draw.
  vgui_event e(vgui_DRAW_OVERLAY);
  glut_insert(e);
}

// do_modifiers sets the modifier bit flags in a vgui_event.
static void do_modifiers(vgui_event &e)
{
  static vgui_modifier last_mods = vgui_MODIFIER_NULL;

  if (e.type == vgui_KEY_PRESS ||
      e.type == vgui_KEY_RELEASE ||
      e.type == vgui_BUTTON_DOWN ||
      e.type == vgui_BUTTON_UP) {
    int mods=glutGetModifiers();
    int modifier = 0;
    if (mods & GLUT_ACTIVE_CTRL)
      modifier |= vgui_CTRL;
    if (mods & GLUT_ACTIVE_SHIFT)
      modifier |= vgui_SHIFT;
    if (mods & GLUT_ACTIVE_ALT)
      modifier |= vgui_META;
    last_mods = vgui_modifier(modifier);
  }

  //
  e.modifier = last_mods;
}

static vgui_key xlate_key_code(unsigned char key)
{
  switch (key) {
  case 127: return vgui_DELETE; // works for me -- fsm
  default: return vgui_key(key);
  }
}

void vgui_glut_slab::keyboard(unsigned char key,int x,int y)
{
  vgui_event e(vgui_KEY_PRESS);
  do_modifiers(e);
  e.key = xlate_key_code(key);
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}

void vgui_glut_slab::keyboard_up(unsigned char key,int x,int y)
{
  vgui_event e(vgui_KEY_RELEASE);
  do_modifiers(e);
  e.key = xlate_key_code(key);
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}

void vgui_glut_slab::mouse(int button,int state,int x,int y)
{
  vgui_event e( (state == GLUT_DOWN) ? vgui_BUTTON_DOWN : vgui_BUTTON_UP );
  do_modifiers(e);

  if (vgui_glut_menu_hack::mouse(button, state, x, y))
    return;

  if (button == GLUT_LEFT_BUTTON)
    e.button = vgui_LEFT;
  else if (button == GLUT_MIDDLE_BUTTON)
    e.button = vgui_MIDDLE;
  else if (button == GLUT_RIGHT_BUTTON)
    e.button = vgui_RIGHT;
  else
    e.button = vgui_BUTTON_NULL;
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}

void vgui_glut_slab::reshape(int width,int height)
{
  vgui_event e;
  e.type = vgui_RESHAPE;
  glut_insert(e);

//   bool f=glut_insert(e);
//   if (!f)
//     glViewport(0, 0, width, height);

//   // call reshape on the sub-contexts :
//   for (unsigned i=0;i<sub_contexts.size();i++) {
//     // FIXME
//     vgui_macro_warning << "subcontext reshape not implemented" << endl;
//   }
}

void vgui_glut_slab::passive_motion(int x,int y)
{
  vgui_event e(vgui_MOTION);
  do_modifiers(e);
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}

void vgui_glut_slab::motion(int x,int y)
{
  vgui_event e(vgui_MOTION);
  do_modifiers(e);
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}

void vgui_glut_slab::timer(int value)
{
  vgui_event e(vgui_TIMER);
  e.timer_id = value;
  glut_insert(e);
}

void vgui_glut_slab::entry(int state)
{
  vgui_event e( (state == GLUT_ENTERED) ? vgui_ENTER : vgui_LEAVE );
  glut_insert(e);
}

void vgui_glut_slab::visibility(int /*state*/)
{
}

void vgui_glut_slab::idle()
{
}

static void xlate_special_key(int key,vgui_event &e)
{
  switch (key) {
  case GLUT_KEY_LEFT: e.key = vgui_CURSOR_LEFT; break;
  case GLUT_KEY_UP: e.key = vgui_CURSOR_UP; break;
  case GLUT_KEY_RIGHT: e.key = vgui_CURSOR_RIGHT; break;
  case GLUT_KEY_DOWN: e.key = vgui_CURSOR_DOWN; break;
  case GLUT_KEY_PAGE_UP: e.key = vgui_PAGE_UP; break;
  case GLUT_KEY_PAGE_DOWN: e.key = vgui_PAGE_DOWN; break;
  case GLUT_KEY_HOME: e.key = vgui_HOME; break;
  case GLUT_KEY_END: e.key = vgui_END; break;
  case GLUT_KEY_INSERT: e.key = vgui_INSERT; break;
  case GLUT_KEY_F1: case GLUT_KEY_F2: case GLUT_KEY_F3: case GLUT_KEY_F4:
  case GLUT_KEY_F5: case GLUT_KEY_F6: case GLUT_KEY_F7: case GLUT_KEY_F8:
  case GLUT_KEY_F9: case GLUT_KEY_F10: case GLUT_KEY_F11: case GLUT_KEY_F12:
    e.key = vgui_key(vgui_F1 + key - GLUT_KEY_F1); break;
  default:
    e.key = vgui_key(key);
    break;
  }
}

void vgui_glut_slab::special(int key,int x,int y)
{
  vgui_event e(vgui_KEY_PRESS);
  do_modifiers(e);
  xlate_special_key(key,e);
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}

void vgui_glut_slab::special_up(int key,int x,int y)
{
  vgui_event e(vgui_KEY_RELEASE);
  do_modifiers(e);
  xlate_special_key(key,e);
  e.wx = x;
  e.wy = y;
  glut_insert(e);
}
