// This is core/vgui/tests/test_acceleration.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpl/vpl.h>

#include <vul/vul_timer.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glx.h>
#include <vgui/internals/vgui_accelerate.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

static int attribs[] = { GLX_RGBA,
                         GLX_RED_SIZE,5,
                         GLX_GREEN_SIZE,6,
                         GLX_BLUE_SIZE,5,
                         GLX_DOUBLEBUFFER,
                         None};

unsigned char global_data[512*512*4];

/*static*/ Bool WaitForNotify(Display* /*d*/, XEvent *e, char *arg)
{
  return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}

void pattern_RGB16 (unsigned char* data)
{
  unsigned short* my_data = (unsigned short *)data;
  unsigned short r = 0xf800;
  unsigned short g = 0x07e0;
#if 0
  unsigned short b = 0x001f;

  for (int y = 0; y < 512; ++y) {
    for (int x = 0; x < 170; ++x) {
      my_data[y*512 +     x] = r;
      my_data[y*512 + 170+x] = g;
      my_data[y*512 + 340+x] = b;
    }
  }
#endif
  for (int y = 0; y < 512; ++y)
    for (int x = 0; x < 512; ++x)
      my_data[y*512 + x] = static_cast<short>(((x << 7) & r) | ((y << 2) & g));
}

void pattern_RGB24 (unsigned char* data)
{
  for (int n=0; n < 512*512*3; ++n) data[n] = 0;

  for (int y = 0; y < 512; ++y) {
    for (int x = 0; x < 170; ++x) {
      data[y*512*3 +         3*x] = 255;
      data[y*512*3 + 170*3 + 3*x + 1] = 255;
      data[y*512*3 + 340*3 + 3*x + 2] = 255;
    }
  }
}

void pattern_RGB32 (unsigned char* data, bool little_endian)
{
  unsigned long r,g,b;
  unsigned long* my_data = (unsigned long *)data;
  if (little_endian) {
    r = 0x000000ff;
    g = 0x0000ff00;
    b = 0x00ff0000;
  }
  else {
    r = 0xff000000;
    g = 0x00ff0000;
    b = 0x0000ff00;
  }
  for (int y = 0; y < 512; ++y) {
    for (int x = 0; x < 170; ++x) {
      my_data[y*512 + x] = r;
      my_data[y*512 + 170 + x] = g;
      my_data[y*512 + 340 + x] = b;
    }
  }
}

int main()
{
  GLenum format;
  GLenum type;
  vgui_accelerate::instance()->vgui_choose_cache_format (&format, &type);
  std::cerr << "chosen format " << std::hex << format << "\nchosen type   " << type << '\n';

  // GLX window code straight out of
  // http://www.eecs.tulane.edu/www/graphics/doc/OpenGL-Man-Pages/glXIntro.html
  std::cerr << "Opening double-buffered, RGBA GLX context...\n\n";
  Display* display = XOpenDisplay(0);
  XVisualInfo* visualinfo = glXChooseVisual (display, DefaultScreen(display), attribs);
  GLXContext context = glXCreateContext (display, visualinfo, 0, GL_TRUE);

  Colormap cmap = XCreateColormap(display, RootWindow(display, visualinfo->screen), visualinfo->visual, AllocNone);

  // create a window
  XSetWindowAttributes swa;
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  Window window = XCreateWindow(display, RootWindow(display, visualinfo->screen),
                                0, 0, 512, 512, 0, visualinfo->depth, InputOutput,
                                visualinfo->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);

  glXMakeCurrent(display, window, context);

  bool little_endian = (ImageByteOrder(display) == LSBFirst);

  std::cerr << "GL_VERSION : " <<  (const char *) glGetString(GL_VERSION) << '\n'
           << "GL_RENDERER : " << (const char *) glGetString(GL_RENDERER) << "\n\n"
           << "X Display -\n"
           << "      byte-order : " << (little_endian ? "little" : "big") << "-endian\n\n"
           << "XVisualInfo -\n"
           << "           depth : " << visualinfo->depth << '\n'
           << "        red-mask : " << std::hex << visualinfo->red_mask << '\n'
           << "      green-mask : " << std::hex << visualinfo->green_mask << '\n'
           << "       blue-mask : " << std::hex << visualinfo->blue_mask << "\n\n"
           << "GL Gets -\n";
  GLint data_int;
  glGetIntegerv(GL_RED_BITS, &data_int);
  std::cerr << "        red-bits : " << data_int << '\n';
  glGetIntegerv(GL_GREEN_BITS, &data_int);
  std::cerr << "      green-bits : " << data_int << '\n';
  glGetIntegerv(GL_BLUE_BITS, &data_int);
  std::cerr << "       blue-bits : " << data_int << '\n';
  glGetIntegerv(GL_ALPHA_BITS, &data_int);
  std::cerr << "      alpha-bits : " << data_int << "\n\n";

#if !VGUI_MESA
  std::cerr << "   You are not using Mesa !!\n";
#endif
#ifndef HAS_HERMES
  std::cerr << "   You do not have Hermes installed !!\n";
#endif

  glViewport( 0, 0, 512, 512 );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0.0, 512, 0.0, 512, -1.0, 1.0 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glRasterPos2i(0,0);

  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Mesa glDrawPixels needs this set to 1 !!
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);

  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glPixelZoom(1.0,1.0);
  glDrawBuffer(GL_BACK_LEFT);

  if (false)
  {
    std::cerr << "\nvgui_glClear - ";
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      vgui_accelerate::instance()->vgui_glClear(GL_COLOR_BUFFER_BIT);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 3000);
    std::cerr << 512*512*draws / (elapsed / 1000.0) << " pixels per second\n";
  }

  if (false)
  {
    std::cerr << "\nglClear - ";
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      glClear(GL_COLOR_BUFFER_BIT);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 3000);
    std::cerr << 512*512*draws / (elapsed / 1000.0) << " pixels per second\n";
  }

  if (true)
  {
    std::cerr << "\nUsing default (RGBA) format with glDrawPixels - ";
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      glDrawPixels(512,512,GL_RGBA,GL_UNSIGNED_BYTE,global_data);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 3000);
    std::cerr << 512*512*draws / (elapsed / 1000.0) << " pixels per second\n";
  }
  if (true)
  {
    std::cerr << "\nUsing recommended format with glDrawPixels - ";
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      glDrawPixels(512,512,format, type, global_data);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 3000);
    std::cerr << 512*512*draws / (elapsed / 1000.0) << " pixels per second\n";
  }
  if (true)
  {
    std::cerr << "\nUsing recommended format with vgui_accelerate::instance()->vgui_glDrawPixels - ";
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      vgui_accelerate::instance()->vgui_glDrawPixels(512,512, format, type, global_data);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 3000);
    std::cerr << 512*512*draws / (elapsed / 1000.0) << " pixels per second\n";
  }

  XMapWindow(display, window);
  XEvent event;
  XIfEvent(display, &event, WaitForNotify, (char*)window);

  std::cerr << "Rendering 16-bit RGB pattern -\n";
  pattern_RGB16(global_data);
  std::cerr << "  glDrawPixels...\n";
  glDrawPixels(512,512,GL_RGB,GLenum(GL_UNSIGNED_SHORT_5_6_5),global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(2);
  std::cerr << "  vgui_accelerate::instance()->vgui_glDrawPixels...\n";
  vgui_accelerate::instance()->vgui_glClear(GL_COLOR_BUFFER_BIT);
  vgui_accelerate::instance()->vgui_glDrawPixels(512,512,GL_RGB,GLenum(GL_UNSIGNED_SHORT_5_6_5),(void *)global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(2);
  glRasterPos2i(45,67);
  std::cerr << "  vgui_accelerate::instance()->vgui_glDrawPixels...\n";
  vgui_accelerate::instance()->vgui_glClear(GL_COLOR_BUFFER_BIT);
  vgui_accelerate::instance()->vgui_glDrawPixels(512,512,GL_RGB,GLenum(GL_UNSIGNED_SHORT_5_6_5),global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
  glPixelZoom(0.4f, 0.6f);
  std::cerr << "  vgui_accelerate::instance()->vgui_glDrawPixels...\n";
  vgui_accelerate::instance()->vgui_glClear(GL_COLOR_BUFFER_BIT);
  vgui_accelerate::instance()->vgui_glDrawPixels(512,512,GL_RGB,GLenum(GL_UNSIGNED_SHORT_5_6_5),global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
  glPixelZoom(1.8f, 0.3f);
  std::cerr << "  vgui_accelerate::instance()->vgui_glDrawPixels...\n";
  vgui_accelerate::instance()->vgui_glClear(GL_COLOR_BUFFER_BIT);
  vgui_accelerate::instance()->vgui_glDrawPixels(512,512,GL_RGB,GLenum(GL_UNSIGNED_SHORT_5_6_5),global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);

#if 0
  std::cerr << "Rendering 24-bit RGB pattern -\n";
  pattern_RGB24(global_data);
  std::cerr << "  glDrawPixels...\n";
  glDrawPixels(512,512,GL_RGB,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
  std::cerr << "  vgui_accelerate::instance()->vgui_glDrawPixels...\n";
  vgui_accelerate::instance()->vgui_glDrawPixels(512,512,GL_RGB,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);

  std::cerr << "Rendering 32-bit RGB pattern -\n";
  pattern_RGB16(global_data);
  std::cerr << "  glDrawPixels...\n";
  glDrawPixels(512,512,GL_RGBA,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
  std::cerr << "  vgui_accelerate::instance()->vgui_glDrawPixels...\n";
  vgui_accelerate::instance()->vgui_glDrawPixels(512,512,GL_RGBA,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
#endif

  vpl_sleep(5);
}
