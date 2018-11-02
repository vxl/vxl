// \author fsm
#include <iostream>
#include <iomanip>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  std::string type_name() const {
    return "front_back_wibbler";
  }
  void print_help_message() const;
};

void front_back_wibbler::print_help_message() const
{
  std::cerr<< __FILE__ " : the amazing front-back wibbler\n"
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
    std::cerr << "null adaptor\n";
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
    std::cerr << "front buffer\n";
    glDrawBuffer(GL_FRONT);
    break;

   case 'b':
    std::cerr << "back buffer\n";
    glDrawBuffer(GL_BACK);
    break;

   case '/':
    std::cerr << "flush\n";
    glFlush();
    glFinish();
    break;

   case 'c':
    std::cerr << "clear\n";
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    break;

   case 'l':
    std::cerr << "line\n";
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
    std::cerr << "point\n";
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
    std::cerr << "copy back -> front\n";
    vgui_utils::copy_back_to_front();
    break;

   case '<':
    std::cerr << "copy front -> back\n";
    vgui_utils::copy_front_to_back();
    break;

   case 's':
    std::cerr << "swap\n";
    ct->swap_buffers();
    break;

   case ' ':
    std::cerr << "some stuff :\n"
             << "   vendor : " << glGetString(GL_VENDOR) << std::endl
             << "   renderer : " << glGetString(GL_RENDERER) << std::endl
             << "   version : " << glGetString(GL_VERSION) << std::endl
             << "   extensions : " << glGetString(GL_EXTENSIONS) << std::endl;
    {
      GLboolean bool_v;
      GLint int_v;

      glGetIntegerv(GL_AUX_BUFFERS, &int_v);
      std::cerr << "   " << int_v << " aux buffers\n";

      glGetBooleanv(GL_DOUBLEBUFFER, &bool_v);
      std::cerr << "   " << (bool_v ? "double" : "single") << " buffered\n";

      glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &int_v);
      std::cerr << "   " << "size of modelview matrix stack is " << int_v << std::endl;

      glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &int_v);
      std::cerr << "   " << "size of projection matrix stack is " << int_v << std::endl;

      std::cerr << "   " << "frame buffer depths:\n";
#define macro(buf) { GLint v;glGetIntegerv(GL_ ## buf ## _BITS,&v); std::cerr<<"   "<<std::setw(3)<<v<<" " #buf <<std::endl; }
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
