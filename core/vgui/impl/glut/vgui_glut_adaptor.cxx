// This is core/vgui/impl/glut/vgui_glut_adaptor.cxx
#include "vgui_glut_adaptor.h"
//:
//  \file
// \author fsm

#include "vgui_glut_window.h"
#include "vgui_glut_popup_impl.h"
#include "menu_hack.h"

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>

#include <vgui/vgui_glut.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/internals/vgui_overlay_helper.h>

//--------------------------------------------------------------------------------

vgui_glut_adaptor::vgui_glut_adaptor( vgui_glut_window *win_, int id_ )
  : vgui_adaptor()
  //
  , id(id_)
  , win(win_)
  //
  , popup_modifier(vgui_MODIFIER_NULL)
  , popup_button(vgui_BUTTON_NULL)
  //, popup_button(vgui_RIGHT)
  //
  , ovl_established( false )
  , ovl_helper( 0 )
  //
  , super(0)
  //
  , popup(0)
{
  all().push_back(this); // register
  register_static_callbacks();
}

vgui_glut_adaptor::~vgui_glut_adaptor() {
  // destroy the overlay helper, if necessary.
  if (ovl_helper)
    delete ovl_helper;
  ovl_helper = 0;

  // destroy the GLUT window through its handle.
  glutDestroyWindow( id );
  id = 0;
  win = 0;

  // deallocate popup.
  if (popup)
    delete popup;
  popup = 0;

  // destroy GLUT sub-contexts.
  for (unsigned i=0; i<sub_contexts.size(); ++i)
    delete sub_contexts[i];
  sub_contexts.clear();

  // remove `this' from `all()'.
  vcl_vector<vgui_glut_adaptor*>::iterator it = vcl_find(all().begin(), all().end(), this);
  assert(it != all().end());
  all().erase(it);
}

//--------------------------------------------------------------------------------

void vgui_glut_adaptor::swap_buffers() {
  //vgui_macro_warning << "glutSwapBuffers()\n";
  int old = glutGetWindow();
  glutSetWindow( id );
  glutSwapBuffers();
  glutSetWindow( old );
}

void vgui_glut_adaptor::make_current() {
  glutSetWindow( id );
}

unsigned vgui_glut_adaptor::get_width() const {
  int old = glutGetWindow();
  glutSetWindow( id );
  unsigned val = glutGet(GLenum(GLUT_WINDOW_WIDTH));
  glutSetWindow( old );
  return val;
}

unsigned vgui_glut_adaptor::get_height() const {
  int old = glutGetWindow();
  glutSetWindow( id );
  unsigned val = glutGet(GLenum(GLUT_WINDOW_HEIGHT));
  glutSetWindow( old );
  return val;
}

vgui_window *vgui_glut_adaptor::get_window() const {
  return win;
}

void vgui_glut_adaptor::post_redraw() {
  int old = glutGetWindow();
  glutSetWindow( id );
  glutPostRedisplay();
  glutSetWindow( old );
}

void vgui_glut_adaptor::post_overlay_redraw() {
  //vcl_cerr << "post_overlay_redraw\n";
  int old = glutGetWindow();
  glutSetWindow( id );
  establish_overlays();
  if (ovl_helper)
    ovl_helper->post_overlay_redraw();
  else
    glutPostOverlayRedisplay();
  glutSetWindow( old );
}

extern void vgui_glut_impl_quit();
void vgui_glut_adaptor::post_destroy() {
  //vgui_macro_warning << "calling exit()\n";
  //exit(1);
  vgui_glut_impl_quit();
}

//--------------------------------------------------------------------------------

extern bool vgui_emulate_overlays;

// This function is designed to be called multiple times, but only the first
// invocation does something. That way, the caller doesn't need to check a
// first-time flag all the time (the routine does it).
void vgui_glut_adaptor::establish_overlays() {
  // make this function idempotent.
  if (ovl_established)
    return;

  // determine whether to use hardware or emulation overlays.
  make_current();
  bool use_hardware;
  if (vgui_emulate_overlays || getenv("vgui_emulate_overlays") != 0)
    use_hardware = false;
  else {
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE );
    use_hardware = glutLayerGet(GLenum(GLUT_OVERLAY_POSSIBLE)) != 0;
    if (!use_hardware) {
      // It could just be that GLUT does not (yet) support RGBA overlays, so
      // let's try asking for a colour indexed overlay plane instead :
      glutInitDisplayMode( GLUT_INDEX | GLUT_SINGLE );
      use_hardware = glutLayerGet(GLenum(GLUT_OVERLAY_POSSIBLE)) != 0;
    }
  }

  // now do it.
  if (use_hardware) {
    glutEstablishOverlay();
    // The callback must be registered after the overlay has been established.
    glutOverlayDisplayFunc(overlay_display_callback);
    // Establishing the layer implicitly makes it the current layer.
    glutUseLayer(GLenum(GLUT_NORMAL));
    vcl_cerr << "GLUT hardware overlay established\n";
  }
  else {
    assert(! ovl_helper);
    ovl_helper = new vgui_overlay_helper(this);
    vcl_cerr << "emulation overlay helper established\n";
  }

  // done.
  ovl_established = true;
}

bool vgui_glut_adaptor::glut_dispatch(vgui_event &e) {
  if (win)
    win->hello_from_adaptor();

  // convert from window to viewport coordinates :
  e.wy = get_height()-1 - e.wy;

  // do not establish the overlay/helper until it is needed.
  if (e.type == vgui_DRAW_OVERLAY)
    establish_overlays();

  // -------------------- using emulation overlays --------------------
  if (ovl_helper)
    return ovl_helper->dispatch(e);

  // -------------------- using glut overlays --------------------
  else {
    // normal draw
    if (e.type == vgui_DRAW) {
      //vgui_macro_warning << "hardware normal redisplay\n";
      //glutUseLayer(GLenum(GLUT_NORMAL));

      bool f = dispatch_to_tableau(e);
#ifdef DUMP_FRAME
      fsm_hook();
#endif
      swap_buffers();
      return f;
    }

    // overlay draw
    else if (e.type == vgui_DRAW_OVERLAY) {
      //vgui_macro_warning << "hardware overlay redisplay\n";
      //glutUseLayer(GLenum(GLUT_OVERLAY));

      // set clear index or color :
      GLboolean is_index_mode;
      glGetBooleanv(GL_INDEX_MODE, &is_index_mode);
      if (is_index_mode) {
        // color index mode
        int index = glutLayerGet(GLenum(GLUT_TRANSPARENT_INDEX));
        {
          static bool once=false;
          if (!once) {
            GLint bits;
            glGetIntegerv(GL_INDEX_BITS, &bits);
            vcl_cerr << __FILE__ ": color index information:\n";
            int cmapsize = glutGet(GLenum(GLUT_WINDOW_COLORMAP_SIZE));
            vcl_cerr << "  color map size is " << cmapsize << vcl_endl
                     << "  transparent color index is " << index << vcl_endl
                     << "  # color index bits is " << bits << vcl_endl;
            // The default color index values appear to be all transparent
            // which is not very helpful, so let's set some more useful
            // values here.
            // - fsm
            //           i  r  g  b
            glutSetColor(0, 0, 0, 0);
            glutSetColor(1, 0, 0, 1); // b
            glutSetColor(2, 0, 1, 0); // g
            glutSetColor(3, 0, 1, 1);
            glutSetColor(4, 1, 0, 0);
            glutSetColor(5, 1, 0, 1); // r
            glutSetColor(6, 1, 1, 0);
            glutSetColor(7, 1, 1, 1);
            int tmp = cmapsize; tmp = 8; ++tmp;
#if 1
            for (int cell=0; cell<tmp; ++cell)
              vcl_cerr << cell << ':'
                       << glutGetColor(cell, GLUT_RED) << ' '
                       << glutGetColor(cell, GLUT_GREEN) << ' '
                       << glutGetColor(cell, GLUT_BLUE) << vcl_endl;
#endif
            once=true;
          }
        }
        glClearIndex(index);
      }
      else // RGBA mode
        glClearColor(0,0,0,0);

      // it's probably sufficient to clear the colour buffer
      // in the overlay plane.
      glClear(GL_COLOR_BUFFER_BIT);

      //
      return dispatch_to_tableau(e);
    }

    // all others
    else
      return dispatch_to_tableau(e);
  }
}

//--------------------------------------------------------------------------------

void vgui_glut_adaptor::register_static_callbacks() {
  make_current();
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

// returns a vcl_list of glut adaptors. having a static data member can cause
// a segfault at module initialization time (linux-egcs).
vcl_vector<vgui_glut_adaptor*> &vgui_glut_adaptor::all() {
  static vcl_vector<vgui_glut_adaptor*> *the_vector = 0;
  if (!the_vector)
    the_vector = new vcl_vector<vgui_glut_adaptor*>;
  return *the_vector;
}

vgui_glut_adaptor *vgui_glut_adaptor::get_adaptor(int window_id) {
  vcl_vector<vgui_glut_adaptor*> &all = vgui_glut_adaptor::all();
  for (unsigned i=0; i<all.size(); ++i)
    if (all[i]->id == window_id)
      return all[i];
  vgui_macro_warning << "window id " << window_id << " is not a glut context\n";
  return 0; // not one of our glut contexts.
}

//--------------------------------------------------------------------------------
//
// per-object (dynamic) callbacks
//

void vgui_glut_adaptor::display() {
  if (glutLayerGet(GLenum(GLUT_LAYER_IN_USE)) != GLUT_NORMAL)
    vgui_macro_warning << "*** current layer is overlay\n";

  // normal draw.
  vgui_event e(vgui_DRAW);
  glut_dispatch(e);
}

void vgui_glut_adaptor::overlay_display() {
  if (glutLayerGet(GLenum(GLUT_LAYER_IN_USE)) != GLUT_OVERLAY)
    vgui_macro_warning << "*** current layer is normal\n";

  {
    GLint isdouble=0;
    glGetIntegerv(GL_DOUBLEBUFFER, &isdouble);
    if (isdouble) // looks suspicious.....
      vgui_macro_warning << "overlay plane is double buffered\n";
  }

  // overlay draw.
  vgui_event e(vgui_DRAW_OVERLAY);
  glut_dispatch(e);
}

// do_modifiers sets the modifier bit flags in a vgui_event.
static void do_modifiers(vgui_event &e) {
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
#if 0
static void do_modifiers(vgui_event &e) {
  int mods=glutGetModifiers(); // can't call this during the motion() callback.
  int modifier = 0;
  if (mods & GLUT_ACTIVE_CTRL)
    modifier |= vgui_CTRL;
  if (mods & GLUT_ACTIVE_SHIFT)
    modifier |= vgui_SHIFT;
  if (mods & GLUT_ACTIVE_ALT)
    modifier |= vgui_META;
  e.modifier = vgui_modifier(modifier);
}
#endif
#if 0
#include <X11/X.h>
extern unsigned __glutModifierMask;
static void do_modifiers(vgui_event &e) { // can call this at any time, though.
  int modifier = 0;
  if (__glutModifierMask & ControlMask)
    modifier |= vgui_CTRL;
  if (__glutModifierMask & (ShiftMask|LockMask))
    modifier |= vgui_SHIFT;
  if (__glutModifierMask & Mod1Mask)
    modifier |= vgui_META;
  e.modifier = vgui_modifier(modifier);
}
#endif

static vgui_key xlate_key_code(unsigned char key) {
  switch (key) {
  case 127: return vgui_DELETE; // works for me -- fsm
  default: return vgui_key(key);
  }
}

void vgui_glut_adaptor::keyboard(unsigned char key,int x,int y) {
  vgui_event e(vgui_KEY_PRESS);
  do_modifiers(e);
  e.key = xlate_key_code(key);
  e.wx = x;
  e.wy = y;
  glut_dispatch(e);
}

void vgui_glut_adaptor::keyboard_up(unsigned char key,int x,int y) {
  vgui_event e(vgui_KEY_RELEASE);
  do_modifiers(e);
  e.key = xlate_key_code(key);
  e.wx = x;
  e.wy = y;
  glut_dispatch(e);
}

void vgui_glut_adaptor::mouse(int button,int state,int x,int y) {
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
  glut_dispatch(e);
}

void vgui_glut_adaptor::reshape(int width,int height) {
  vgui_event e;
  e.type = vgui_RESHAPE;
  bool f=glut_dispatch(e);
  if (!f) {
    glViewport(0, 0, width, height);
    glScissor (0, 0, width, height);
  }

  // call reshape on the sub-contexts :
  for (unsigned i=0;i<sub_contexts.size();i++) {
    // FIXME
    vgui_macro_warning << "subcontext reshape not implemented\n";
  }
}

void vgui_glut_adaptor::passive_motion(int x,int y) {
  vgui_event e(vgui_MOTION);
  do_modifiers(e);
  e.wx = x;
  e.wy = y;
  glut_dispatch(e);
}

void vgui_glut_adaptor::motion(int x,int y) {
  vgui_event e(vgui_MOTION);
  do_modifiers(e);
  e.wx = x;
  e.wy = y;
  glut_dispatch(e);
}

void vgui_glut_adaptor::timer(int value) {
  vgui_event e(vgui_TIMER);
  e.timer_id = value;
  glut_dispatch(e);
}

void vgui_glut_adaptor::entry(int state) {
  vgui_event e( (state == GLUT_ENTERED) ? vgui_ENTER : vgui_LEAVE );
  glut_dispatch(e);
}

void vgui_glut_adaptor::visibility(int /*state*/)  {
}

static void xlate_special_key(int key,vgui_event &e) {
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

void vgui_glut_adaptor::special(int key,int x,int y)  {
  vgui_event e(vgui_KEY_PRESS);
  do_modifiers(e);
  xlate_special_key(key,e);
  e.wx = x;
  e.wy = y;
  glut_dispatch(e);
}

void vgui_glut_adaptor::special_up(int key,int x,int y)  {
  vgui_event e(vgui_KEY_RELEASE);
  do_modifiers(e);
  xlate_special_key(key,e);
  e.wx = x;
  e.wy = y;
  glut_dispatch(e);
}

//--------------------------------------------------------------------------------

// This is a the 'last_minute_change_callback' pass to menu_hack. It is called
// just before glut starts popping up the menu with the given id. See menu_hack
// for more details.
void vgui_glut_adaptor::pre_menu_hook(int menu_id) {
  // Find out which glut adaptor is using the given menu id.
  // Then ask it to update the glut menu.
  for (unsigned i=0; i<all().size(); ++i) {
    vgui_glut_adaptor *ct = all()[i];
    if (ct->popup && ct->popup->menu_id==menu_id) {
      ct->make_popup();
      return;
    }
  }
  vgui_macro_warning << "unrecognised menu id " << menu_id << vcl_endl;
}

void vgui_glut_adaptor::make_popup() {
  make_current();

  // make a glut version of the menu :
  if (popup)
    popup->clear();
  else
    popup =  new vgui_glut_popup_impl;
  {
    vgui_popup_params params;
    params.x = 0; // FIXME
    params.y = 0; // FIXME
    params.recurse = true;
    vgui_menu menu;
    if (win)
      menu.include( win->menubar );
    menu.include( get_total_popup(params) );
    popup->build( menu );
  }

  // translate vgui button to GLUT button :
  int button = 0;
  switch (popup_button) {
  case vgui_LEFT:
    button = GLUT_LEFT_BUTTON;
    break;
  case vgui_MIDDLE:
    button = GLUT_MIDDLE_BUTTON;
    break;
  default:
    vgui_macro_warning << "unknown vgui_button - assuming right button\n";
  case vgui_RIGHT:
    button = GLUT_RIGHT_BUTTON;
    break;
  }
  //vcl_cerr << "button = " << button << vcl_endl;

  // translate vgui modifiers to GLUT modifiers :
  int mods = 0;
  if (popup_modifier & vgui_CTRL)
    mods |= GLUT_ACTIVE_CTRL;
  if (popup_modifier & vgui_SHIFT)
    mods |= GLUT_ACTIVE_SHIFT;
  if (popup_modifier & vgui_ALT)
    mods |= GLUT_ACTIVE_ALT;
  //vcl_cerr << "mods = " << mods << vcl_endl;

  // bind buttons and set the menu_hack callback.
  vgui_glut_menu_hack::bind(button, mods, popup->menu_id);
  vgui_glut_menu_hack::last_minute_change_callback = pre_menu_hook;
}

void vgui_glut_adaptor::bind_popups(vgui_modifier mod, vgui_button but) {
  popup_button = but;
  popup_modifier = mod;
  this->make_popup();
}

//--------------------------------------------------------------------------------

// Static callbacks. First the special cases :

//: post timeout events
struct vgui_glut_adaptor_callback_data
{
  vgui_glut_adaptor *org;
  int val;
};

#include <vcl_utility.h>
#include <vcl_list.h>
typedef vcl_pair<void*, int> pair_Pv_i;
typedef vcl_list<pair_Pv_i> list_Pv_i;
static list_Pv_i *timer_posts = 0;

void vgui_glut_adaptor::post_timer(float timeout, int name)
{
  vgui_glut_adaptor_callback_data *ff = new vgui_glut_adaptor_callback_data;   // <*> acquire resource
  ff->org = this;
  ff->val = name;

  // convert the pointer 'ff' to an int 'value'.
  int value = 0;
  if (!timer_posts)
    timer_posts = new list_Pv_i;
  for (list_Pv_i::iterator i=timer_posts->begin(); i!=timer_posts->end(); ++i)
    if (value <= (*i).second)
      value = (*i).second + 1;
  timer_posts->push_front(pair_Pv_i(ff, value));

  // pass 'value' to the GLUT api.
  glutTimerFunc(int(timeout*1000), vgui_glut_adaptor::timer_callback, value);
}

void vgui_glut_adaptor::timer_callback(int value)
{
  // convert 'value' back to a pointer 'ff'.
  vgui_glut_adaptor_callback_data *ff = 0;
  assert(timer_posts);
  for (list_Pv_i::iterator i=timer_posts->begin(); i!=timer_posts->end(); ++i)
    if (value == (*i).second) {
      ff = static_cast<vgui_glut_adaptor_callback_data*>( (*i).first );
      timer_posts->erase(i);
      break;
    }
  assert(ff);

  ff->org->timer(ff->val);
  delete ff;                               // <*> release resource
}

//------------------------------------------------------------

//: called when the menu status changes
void vgui_glut_adaptor::menustatus_callback(int status, int x, int y) {
  vgui_glut_menu_hack::menustatus(status,x,y);
}

// dispatch macro which works in all other cases :
#define implement_static_callback(name, proto, args) \
void vgui_glut_adaptor::name##_callback proto { \
 vgui_glut_adaptor *v=get_adaptor(glutGetWindow()); \
 if (v) \
   v->name args; \
 else \
   vcl_abort(); \
}
implement_static_callback(display,(),());
implement_static_callback(overlay_display,(),());
implement_static_callback(reshape,(int width,int height),(width,height));
implement_static_callback(keyboard,(unsigned char key,int x,int y),(key,x,y));
implement_static_callback(keyboard_up,(unsigned char key,int x,int y),(key,x,y));
implement_static_callback(mouse,(int button,int state,int x,int y),(button,state,x,y));
implement_static_callback(visibility,(int state),(state));
implement_static_callback(motion,(int x,int y),(x,y));
implement_static_callback(entry,(int state),(state));
implement_static_callback(passive_motion,(int x,int y),(x,y));
implement_static_callback(special,(int key,int x,int y),(key,x,y));
implement_static_callback(special_up,(int key,int x,int y),(key,x,y));

//--------------------------------------------------------------------------------

#ifdef DUMP_FRAME
#include <vul/vul_sprintf.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_rgba.h>
#include <vil1/vil1_memory_image_of.h>
static
void fsm_dump(char const *file)
{
  // get viewport size
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned x = vp[0];
  unsigned y = vp[1];
  unsigned w = vp[2];
  unsigned h = vp[3];

  // It's easier to get the buffer in vil1_rgba format and then convert to
  // RGB, because that avoids alignment problems with glReadPixels.
  static vil1_rgba<GLubyte> *pixels = 0;
  if (! pixels)
    pixels = new vil1_rgba<GLubyte>[ w * h ];

  //
  glPixelZoom(1,1);
  glPixelTransferi(GL_MAP_COLOR,0);
  glPixelTransferi(GL_RED_SCALE,1);   glPixelTransferi(GL_RED_BIAS,0);
  glPixelTransferi(GL_GREEN_SCALE,1); glPixelTransferi(GL_GREEN_BIAS,0);
  glPixelTransferi(GL_BLUE_SCALE,1);  glPixelTransferi(GL_BLUE_BIAS,0);

  //
  glPixelStorei(GL_PACK_ALIGNMENT,1);   // byte alignment.
  glPixelStorei(GL_PACK_ROW_LENGTH,0);  // use default value (the arg to pixel routine).
  glPixelStorei(GL_PACK_SKIP_PIXELS,0); //
  glPixelStorei(GL_PACK_SKIP_ROWS,0);   //

  // read from the *back buffer*.
  glReadBuffer(GL_BACK);

  //
  glReadPixels(x, y,             //
               w, h,             //
               GL_RGBA,          // format
               GL_UNSIGNED_BYTE, // type
               pixels);

  // glReadPixels() reads the pixels from the bottom of the viewport up.
  // Copy them into an vil1_memory_image_of in the other order :
  static vil1_memory_image_of<vil1_rgb<GLubyte> > colour_buffer;
  colour_buffer.resize(w, h);

  for (unsigned yy=0; yy<h; ++yy)
    for (unsigned xx=0; xx<w; ++xx) {
      colour_buffer(xx, h-1-yy).r = pixels[xx + w*yy].r;
      colour_buffer(xx, h-1-yy).g = pixels[xx + w*yy].g;
      colour_buffer(xx, h-1-yy).b = pixels[xx + w*yy].b;
    }

  //
  vil1_save(colour_buffer, file, "pnm");
}
bool fsm_hook_flag = false;
static
void fsm_hook()
{
  if (fsm_hook_flag) {
    static int frame_counter = 0;
    fsm_dump(vul_sprintf("/tmp/dump%03d.pnm", frame_counter++).c_str());
  }
}
#endif
