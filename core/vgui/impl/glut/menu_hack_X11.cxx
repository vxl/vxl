/*
  fsm
*/
#include "menu_hack.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glut.h>

#include <X11/Xlib.h>

// The purpose of this class is to fool GLUT into popping up
// menus on more special conditions than mouse presses. I
// works by intercepting the desired event (modifier+button),
// registering a GLUT menu on that button and then putting a
// fake X button event back onto the event stream.
//
// This works with both shared and non-shared GLUT libraries,
// but it may break when linked against with newer versions of
// GLUT.

//--------------------------------------------------------------------------------

// see glutint.h (taken from glut source distribution) for the
// rest of this structure.
struct GLUTwindow
{
  int num;              /* Small integer window id (0-based). */

  /* Window system related state. */
#if defined(_WIN32) && !defined(__CYGWIN__)
  int pf;               /* Pixel format. */
  HDC hdc;              /* Window's Win32 device context. */
#endif
  Window win;           /* X window for GLUT window */
  // *** there's more, but we don't need it..... ***
};

// we use these extern global variables in libGLUT to do the trickery :
extern    Display *__glutDisplay;
extern GLUTwindow *__glutCurrentWindow;
extern GLUTwindow *__glutMenuWindow;

//--------------------------------------------------------------------------------

int  vgui_glut_menu_hack::glut_button = 0;
bool vgui_glut_menu_hack::active = false;
void (*vgui_glut_menu_hack::last_minute_change_callback)(int menu_id) = 0;

//--------------------------------------------------------------------------------

struct vgui_glut_menu_hack_bind_entry
{
  int button;
  int mods;
  int menu_id;
};

static vgui_glut_menu_hack_bind_entry default_entries[] = {
  {GLUT_LEFT_BUTTON  ,0                ,0},
  {GLUT_LEFT_BUTTON  ,GLUT_ACTIVE_SHIFT,0},
  {GLUT_LEFT_BUTTON  ,GLUT_ACTIVE_CTRL ,0},
  {GLUT_LEFT_BUTTON  ,GLUT_ACTIVE_ALT  ,0},
  {GLUT_MIDDLE_BUTTON,0                ,0},
  {GLUT_MIDDLE_BUTTON,GLUT_ACTIVE_SHIFT,0},
  {GLUT_MIDDLE_BUTTON,GLUT_ACTIVE_CTRL ,0},
  {GLUT_MIDDLE_BUTTON,GLUT_ACTIVE_ALT  ,0},
  {GLUT_RIGHT_BUTTON ,0                ,0},
  {GLUT_RIGHT_BUTTON ,GLUT_ACTIVE_SHIFT,0},
  {GLUT_RIGHT_BUTTON ,GLUT_ACTIVE_CTRL ,0},
  {GLUT_RIGHT_BUTTON ,GLUT_ACTIVE_ALT  ,0},
};

const unsigned int table_size = sizeof(default_entries)/sizeof(default_entries[0]);

struct vgui_glut_menu_hack::per_window_record
{
  vgui_glut_menu_hack_bind_entry entries[ table_size ];
  per_window_record() {
    for (unsigned i=0;i<table_size; ++i)
      entries[i] = default_entries[i];
  }
};

static vgui_glut_menu_hack::per_window_record * get_current_record()
{
  static vcl_vector<vgui_glut_menu_hack::per_window_record *> records;

  unsigned win = glutGetWindow();
  if (win == 0)
    return 0;
  while (win >= records.size())
    records.push_back( (vgui_glut_menu_hack::per_window_record*)0 ); // gcc 2.7 needs this cast

  if (records[win] == 0) {
#ifdef DEBUG
    vcl_cerr << __FILE__ " : create record for window " << win << vcl_endl;
#endif
    records[win] = new vgui_glut_menu_hack::per_window_record;
  }

  return records[win];
}

//--------------------------------------------------------------------------------

int vgui_glut_menu_hack::find_index(int button, int mods)
{
  const per_window_record *rec = get_current_record();
  if (!rec)
    return -1;
  for (unsigned i=0; i<table_size; i++)
    if (button==rec->entries[i].button && mods==rec->entries[i].mods)
      return i;
  vcl_cerr << __FILE__ " : invalid button/modifier combination " << button << ' ' << mods << vcl_endl;
  return -1;
}

bool vgui_glut_menu_hack::mouse(int button, int state, int x, int y)
{
  if (state != GLUT_DOWN)
    return false;

  int mods = glutGetModifiers();

  int index = find_index(button, mods);

  if (index<0)
    return false; // invalid modifier combination.

  const per_window_record *rec = get_current_record();
  if (!rec)
    return false;

  if (rec->entries[index].menu_id==0)
    return false; // no menu bound.

  // allow client to change the menu if desired :
  if (last_minute_change_callback)
    last_minute_change_callback(rec->entries[index].menu_id);

  active = true;
  glut_button = button;
#ifdef DEBUG
  vcl_cerr << "active\n";
#endif

  // attach the required button to the menu
  {
    int old_id = glutGetMenu();
    int tmp_id = rec->entries[index].menu_id;
    glutSetMenu(tmp_id);
    glutAttachMenu(glut_button);
    if (old_id)
      glutSetMenu(old_id);
  }

  { // push another button press onto the event stream :
    static XEvent event;
    event.xbutton.type = ButtonPress;
    event.xbutton.serial = 0x12345678; // ?
    event.xbutton.send_event = 0;
    event.xbutton.display = __glutDisplay;
    event.xbutton.window = __glutCurrentWindow->win;
    event.xbutton.root = 0; // ?
    event.xbutton.subwindow = 0; // ?
    event.xbutton.time = 0; // ?
    event.xbutton.x = x;
    event.xbutton.y = y;
    event.xbutton.x_root = glutGet(GLenum(GLUT_WINDOW_X)) + x; // ?? but this
    event.xbutton.y_root = glutGet(GLenum(GLUT_WINDOW_Y)) + y; // works ??
    event.xbutton.state = 0;
    if (glut_button == GLUT_LEFT_BUTTON)
      event.xbutton.button = Button1;
    if (glut_button == GLUT_MIDDLE_BUTTON)
      event.xbutton.button = Button2;
    if (glut_button == GLUT_RIGHT_BUTTON)
      event.xbutton.button = Button3;
    event.xbutton.same_screen = 1; // ?

    XPutBackEvent(__glutDisplay, &event);
  }
  // the button press we pushed should bring up the menu.

  return true;
}

void vgui_glut_menu_hack::menustatus(int status,int /*x*/,int /*y*/)
{
  if (active  &&  status == GLUT_MENU_NOT_IN_USE) {
    glutDetachMenu(glut_button);
#ifdef DEBUG
    vcl_cerr << "purged\n";
#endif
    active = false;
  }
}

//--------------------------------------------------------------------------------

void vgui_glut_menu_hack::bind  (int button, int mods, int menu_id)
{
  int index = find_index(button, mods);
  if (index<0)
    return;
  else {
    per_window_record *rec = get_current_record();
    if (!rec)
      return;
#ifdef DEBUG
    vcl_cerr << "bind : " << glutGetWindow() << ' ' <<  button << ' ' << mods << ' ' << menu_id <<  vcl_endl;
#endif
    rec->entries[index].menu_id = menu_id;
  }
}
