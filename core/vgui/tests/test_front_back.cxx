// \author fsm
#include <vcl_iostream.h>
#include <vcl_iomanip.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>

// The front_back_wibbler is a tablo which, given an OpenGL context
// traps keypresses and executes certain gl commands in response.
// The gl commands all relate to double buffering. This is a debugging
// tool.

class front_back_wibbler : public vgui_tableau
{
 public:
  front_back_wibbler() {}

  bool handle(vgui_event const &);
  vcl_string type_name() const {
    return "front_back_wibbler";
  }
  void print_help_message() const;
};

void front_back_wibbler::print_help_message() const
{
  vcl_cerr<< __FILE__ " : the amazing front-back wibbler\n"
          << "'f' : draw into front buffer\n"
          << "'b' : draw into back buffer\n"
          << "'/' : glFlush(); glFinish();\n"
          << "'c' : clear buffer\n"
          << "'l' : draw a line\n"
          << "'p' : draw a point\n"
          << "'<' : copy front buffer into back buffer\n"
          << "'>' : copy back buffer into front buffer\n"
          << "'s' : swap front and back buffers\n"
          << "' ' : print some stuff\n\n"
          << "NB. Mesa SwapBuffers will simply blit the last back buffer into\n"
          << "the current front buffer, rather than actually perform a swap.\n\n";
}

bool front_back_wibbler::handle(const vgui_event &e)
{
  vgui_adaptor *ct = e.origin;
  if (!ct) {
    vcl_cerr << "null adaptor\n";
    return false;
  }

  if (e.type != vgui_KEY_PRESS)
    return false;

  ct->make_current();
  switch (e.key)
  {
   case '?':
    print_help_message();
    break;

   case 'f':
    vcl_cerr << "front buffer\n";
    glDrawBuffer(GL_FRONT);
    break;

   case 'b':
    vcl_cerr << "back buffer\n";
    glDrawBuffer(GL_BACK);
    break;

   case '/':
    vcl_cerr << "flush\n";
    glFlush();
    glFinish();
    break;

   case 'c':
    vcl_cerr << "clear\n";
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    break;

   case 'l':
    vcl_cerr << "line\n";
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(-.5,-.5);
    glVertex2f(+.5,+.5);
    glEnd();
    break;

   case 'p':
    vcl_cerr << "point\n";
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1,1,1);
    glPointSize(4.0);
    glBegin(GL_POINTS);
    glVertex2f(0,0);
    glEnd();
    break;

   case '>':
    vcl_cerr << "copy back -> front\n";
    vgui_utils::copy_back_to_front();
    break;

   case '<':
    vcl_cerr << "copy front -> back\n";
    vgui_utils::copy_front_to_back();
    break;

   case 's':
    vcl_cerr << "swap\n";
    ct->swap_buffers();
    break;

   case ' ':
    vcl_cerr << "some stuff :\n"
             << "   vendor : " << glGetString(GL_VENDOR) << vcl_endl
             << "   renderer : " << glGetString(GL_RENDERER) << vcl_endl
             << "   version : " << glGetString(GL_VERSION) << vcl_endl
             << "   extensions : " << glGetString(GL_EXTENSIONS) << vcl_endl;
    {
      GLboolean bool_v;
      GLint int_v;

      glGetIntegerv(GL_AUX_BUFFERS, &int_v);
      vcl_cerr << "   " << int_v << " aux buffers\n";

      glGetBooleanv(GL_DOUBLEBUFFER, &bool_v);
      vcl_cerr << "   " << (bool_v ? "double" : "single") << " buffered\n";

      glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &int_v);
      vcl_cerr << "   " << "size of modelview matrix stack is " << int_v << vcl_endl;

      glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &int_v);
      vcl_cerr << "   " << "size of projection matrix stack is " << int_v << vcl_endl;

      vcl_cerr << "   " << "frame buffer depths:\n";
#define macro(buf) { GLint v;glGetIntegerv(GL_ ## buf ## _BITS,&v); vcl_cerr<<"   "<<vcl_setw(3)<<v<<" " #buf <<vcl_endl; }
      // Number of bits per R, G, B, or A component in  the color buffers
      macro(RED);
      macro(GREEN);
      macro(BLUE);
      macro(ALPHA);
      macro(INDEX);
      // Number of bits per pixel in the depth buffer
      macro(DEPTH);
      // Number of bits per pixel in the stencil buffer
      macro(STENCIL);
      // Number of bits per R, G, B, or A component in  the accumulation buffer
      macro(ACCUM_RED);
      macro(ACCUM_GREEN);
      macro(ACCUM_BLUE);
      macro(ACCUM_ALPHA);
#undef macro
    }
    break;

   default:
    break;
  }
  return true;
}

//------------------------------------------------------------------------------

int main(int argc,char **argv)
{
  vgui::init(argc, argv);
  return vgui::run(new front_back_wibbler, 256, 256, __FILE__);
}
