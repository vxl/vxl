// This is core/vgui/tests/test_drawpix_speed.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpl/vpl.h>

#include <vul/vul_timer.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


#if VGUI_MESA
#  include <GL/xmesa.h>
#endif

static int attribs[] = { GLX_RGBA,
                         GLX_RED_SIZE,5,
                         GLX_GREEN_SIZE,6,
                         GLX_BLUE_SIZE,5,
                         GLX_DOUBLEBUFFER,
                         None};

unsigned char global_data[512*512*4];

#if 0 // unused static function
static Bool WaitForNotify(Display *d, XEvent *e, char *arg)
{
  return e->type == MapNotify && e->xmap.window == (Window)arg;
}
#endif // 0

double fps_gl (GLenum pack_type, GLenum pix_type)
{
  int draws = 0;
  int elapsed;
  vul_timer timer;
  timer.mark();
  do {
    glDrawPixels(512,512,pack_type,pix_type,global_data);
    ++draws;
    elapsed = timer.real();
  } while (elapsed < 3000);
  return draws * 1000.0 / elapsed;
}


#ifdef HAS_HERMES
#include <Hermes/Hermes.h>
double fps_hermes (float src_scale, float dest_scale, HermesFormat* src_format, XImage* backbuffer)
{
  HermesFormat* dest_format =
    Hermes_FormatNew(backbuffer->bits_per_pixel, backbuffer->red_mask, backbuffer->green_mask, backbuffer->blue_mask, 0, 0);
  HermesHandle converter = Hermes_ConverterInstance(HERMES_CONVERT_NORMAL);
  Hermes_ConverterRequest(converter, src_format, dest_format);
  int draws=0;
  int elapsed;
  vul_timer timer;
  int bytes_per_pixel = src_format->bits / 8;
  int succeed;
  do {
    succeed = Hermes_ConverterCopy(converter, global_data,
                                   0, 0, (int)(src_scale*512), (int)(src_scale*512), 512*bytes_per_pixel, backbuffer->data,
                                   0, 0, (int)(dest_scale*512), (int)(dest_scale*512), backbuffer->bytes_per_line);
    if (!succeed) return 0;
    elapsed = timer.real();
    ++draws;
  } while (elapsed < 3000);
  Hermes_ConverterReturn(converter);
  Hermes_FormatFree(dest_format);
  Hermes_FormatFree(src_format);
  return draws * 1000.0 / elapsed;
}

double fps_hermes_grey (float src_scale, float dest_scale, XImage* backbuffer)
{
  // special paletted case for 8-bit mode
  HermesHandle palette = Hermes_PaletteInstance();
  HermesFormat* dest_format =
    Hermes_FormatNew(backbuffer->bits_per_pixel, backbuffer->red_mask, backbuffer->green_mask, backbuffer->blue_mask, 0, 0);
  HermesFormat* src_format = Hermes_FormatNew(8,0,0,0,0,1);
  HermesHandle converter = Hermes_ConverterInstance(HERMES_CONVERT_NORMAL);
  Hermes_ConverterRequest(converter, src_format, dest_format);
  Hermes_ConverterPalette(converter,palette,palette); // last param is a dummy
  int draws=0;
  int elapsed;
  vul_timer timer;
  int succeed;
  do {
    succeed = Hermes_ConverterCopy(converter, global_data,
                                   0, 0, (int)(src_scale*512), (int)(src_scale*512), 512*1, backbuffer->data,
                                   0, 0, (int)(dest_scale*512), (int)(dest_scale*512), backbuffer->bytes_per_line);
    if (!succeed) return 0;
    elapsed = timer.real();
    ++draws;
  } while (elapsed < 3000);
  Hermes_ConverterReturn(converter);
  Hermes_PaletteReturn(palette);
  Hermes_FormatFree(src_format);
  Hermes_FormatFree(dest_format);
  return draws * 1000.0 / elapsed;
}

#endif // HAS_HERMES

void pattern_grey (unsigned char* data)
{
  for (int y=0; y < 512; ++y) {
    for (int x=0; x < 256; ++x) {
      data[y*512 + 2*x] = data[y*512 + 2*x + 1] = static_cast<unsigned char>(x);
    }
  }
}

void pattern_RGB16 (unsigned char* data, bool little_endian)
{
  unsigned short r,g,b;
  unsigned short* my_data = (unsigned short *)data;
  if (little_endian) {
    r = 0x001f;
    g = 0x07e0;
    b = 0xf800;
  }
  else {
    r = 0xf800;
    g = 0x07e0;
    b = 0x001f;
  }
  for (int y = 0; y < 512; ++y) {
    for (int x = 0; x < 170; ++x) {
      my_data[y*512 +     x] = r;
      my_data[y*512 + 170+x] = g;
      my_data[y*512 + 340+x] = b;
    }
  }
}

void pattern_RGB24 (unsigned char* data, bool /*little_endian*/)
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


// Try to do the #ifdef .. #endif madness once and for all here :
static struct
{
  GLenum format;
  GLenum type;
  char const *nfixed;
  char const *pretty;
}

ft_tab[] =
{
  {GL_LUMINANCE,GL_UNSIGNED_BYTE, "LUM   ", "8bit greyscale"},
  {GL_RGB,GL_UNSIGNED_BYTE,       "RGB   ", "24bit 888 RGB"},
  {GL_RGBA,GL_UNSIGNED_BYTE,      "RGBA  ", "32bit 8888 RGBA"},
#if defined(GL_VERSION_1_2)
  {GLenum(GL_RGB),GLenum(GL_UNSIGNED_SHORT_5_6_5),"RGB565", "16bit 565 RGB"},
  {GLenum(GL_BGR),GLenum(GL_UNSIGNED_BYTE),       "BGR   ", "24bit 888 BGR"},
  {GLenum(GL_BGRA),GLenum(GL_UNSIGNED_BYTE),      "BGRA  ", "32bit 8888 BGRA"},
#endif
#if defined(GL_ABGR_EXT) || defined(GL_EXT_abgr)
  {GLenum(GL_ABGR_EXT),GLenum(GL_UNSIGNED_BYTE),  "ABGR  ", "32bit ABGR"},
#endif
  {GL_NONE, GL_NONE, 0, 0}
};

static const int ft_size = sizeof(ft_tab)/sizeof(ft_tab[0]) - 1;


int main()
{
  // GLX window code straight out of http://www.eecs.tulane.edu/www/graphics/doc/OpenGL-Man-Pages/glXIntro.html
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
  Window window = XCreateWindow(display, RootWindow(display,visualinfo->screen),
                                0, 0, 512, 512, 0, visualinfo->depth, InputOutput,
                                visualinfo->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);

  glXMakeCurrent(display, window, context);

  bool little_endian = (ImageByteOrder(display) == LSBFirst);
  std::cerr << "GL_VERSION : " <<  (const char*) glGetString(GL_VERSION) << '\n'
           << "GL_RENDERER : " << (const char*) glGetString(GL_RENDERER)<< "\n\n"
           << "X Display -\n"
           << "      byte-order : " << (little_endian ? "little" : "big") << "-endian\n\n"
           << "XVisualInfo -\n"
           << "           depth : " << visualinfo->depth << std::endl
           << "        red-mask : " << std::hex << visualinfo->red_mask << '\n'
           << "      green-mask : " << std::hex << visualinfo->green_mask << '\n'
           << "       blue-mask : " << std::hex << visualinfo->blue_mask << "\n\n"
           << "GL Gets -\n";
  GLint data_int;
  glGetIntegerv(GL_RED_BITS, &data_int);
  std::cerr << "        red-bits : " << data_int << std::endl;
  glGetIntegerv(GL_GREEN_BITS, &data_int);
  std::cerr << "      green-bits : " << data_int << std::endl;
  glGetIntegerv(GL_BLUE_BITS, &data_int);
  std::cerr << "       blue-bits : " << data_int << std::endl;
  glGetIntegerv(GL_ALPHA_BITS, &data_int);
  std::cerr << "      alpha-bits : " << data_int << std::endl;

#if VGUI_MESA
  {
    std::cerr << std::endl;
    XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
    Pixmap p;
    XImage* backbuffer;
    XMesaGetBackBuffer(mesabuf, &p, &backbuffer);

    bool little_endian = (backbuffer->byte_order == LSBFirst);
    std::cerr << "Mesa backbuffer XImage -\n"
             << "           depth : " << backbuffer->depth << std::endl
             << "  bits-per-pixel : " << backbuffer->bits_per_pixel << std::endl
             << "      byte_order : " << (little_endian ? "little" : "big") << "-endian\n"
             << "  bytes-per-line : " << backbuffer->bytes_per_line << std::endl
             << "        red-mask : " << std::hex << backbuffer->red_mask << '\n'
             << "      green-mask : " << std::hex << backbuffer->green_mask << '\n'
             << "       blue-mask : " << std::hex << backbuffer->blue_mask << '\n';
  }
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
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // gl_direct_DrawPixels needs this set to 1 !!
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 512);

  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glPixelZoom(1.0,1.0);
  glDrawBuffer(GL_BACK_LEFT);

#if 0 // commented out
  // Render from all formats to check endian settings
  XMapWindow(display, window);
  XEvent event;
  XIfEvent(display, &event, WaitForNotify, (char*)window);
  std::cerr << "Rendering from 8-bit grey-level...\n";
  pattern_grey(global_data);
  glDrawPixels(512,512,GL_LUMINANCE,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
#if defined(GL_VERSION_1_2)
  std::cerr << "Rendering from 16-bit 565 RGB...\n";
  pattern_RGB16(global_data, little_endian);
  glDrawPixels(512,512,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
#endif
  std::cerr << "Rendering from 24-bit 888 RGB...\n";
  pattern_RGB24(global_data, little_endian);
  glDrawPixels(512,512,GL_RGB,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
  std::cerr << "Rendering from 32-bit 8888 RGBA...\n";
  pattern_RGB32(global_data, little_endian);
  glDrawPixels(512,512,GL_RGBA,GL_UNSIGNED_BYTE,global_data);
  glXSwapBuffers(display, window);
  vpl_sleep(1);
#endif // end commented out

  {
    std::cerr << "\nglClear - ";
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      glClear(GL_COLOR_BUFFER_BIT);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 5000);
    std::cerr << 512*512*draws / (elapsed / 1000.0) << " pixels per second\n";
  }
  {
    std::cerr << "\nglDrawPixels -\n";
    double fps;
    std::cerr << "source -";
    for (int i=0; i<ft_size; ++i)
      std::cerr << "    " << ft_tab[i].nfixed;
    std::cerr << std::endl
             << "source -    LUM       RGB565    RGB       BGR       RGBA      BGRA      ABGR\n"
             << "zoom 1.00x  ";
    for (int i=0; i<ft_size; ++i) {
      fps = fps_gl(ft_tab[i].format, ft_tab[i].type);
      std::cerr << 512*512*fps << "   ";
    }
    std::cerr << "\nzoom 1.90x  ";
    glPixelZoom(1.9f, 1.9f);
    for (int i=0; i<ft_size; ++i) {
      fps = fps_gl(ft_tab[i].format, ft_tab[i].type);
      std::cerr << 512*512*fps << "   ";
    }
    std::cerr << "\nzoom 0.51x  ";
    glPixelZoom(0.51f, 0.51f);
    for (int i=0; i<ft_size; ++i) {
      fps = fps_gl(ft_tab[i].format, ft_tab[i].type);
      std::cerr << 0.51*0.51*512*512*fps << "   ";
    }
    std::cerr << std::endl;
  }

#if defined(HAS_HERMES) && VGUI_MESA
  {
    XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
    Pixmap p;
    XImage* backbuffer;
    XMesaGetBackBuffer(mesabuf, &p, &backbuffer);

  Hermes_Init();
  HermesFormat* dest_format =
    Hermes_FormatNew(backbuffer->bits_per_pixel, backbuffer->red_mask, backbuffer->green_mask, backbuffer->blue_mask, 0, 0);
  {
    std::cerr << "\nHermesClear - ";
    HermesHandle clearer = Hermes_ClearerInstance();
    Hermes_ClearerRequest (clearer, dest_format);
    int draws=0;
    int elapsed;
    vul_timer timer;
    do {
      Hermes_ClearerClear(clearer,backbuffer->data,0,0,512,512,backbuffer->bytes_per_line,0,0,0,0);
      elapsed = timer.real();
      ++draws;
    } while (elapsed < 3000);
    std::cerr << (512*512*draws) / (elapsed / 1000.0) << " pixels per second\n";
    Hermes_ClearerReturn(clearer);
  }
  {
    std::cerr << "\nHermesConverter -\n";
    HermesFormat* src_format;
    double fps;
    std::cerr <<"source -    LUM      OxRGB565 OxRGB    OxBGR    Ox_RGB   0x_BGR\n";

    std::cerr << "zoom 1.00x  ";
    fps = fps_hermes_grey(1.0, 1.0, backbuffer);
    std::cerr << 12*512*fps << "  ";

    src_format = Hermes_FormatNew(16, 0xf800, 0x07e0, 0x001f, 0, 0);
    fps = fps_hermes(1.0, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(24, 0xff0000, 0xff00, 0xff, 0, 0);
    fps = fps_hermes(1.0, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(24, 0xff, 0xff00, 0xff0000, 0, 0);
    fps = fps_hermes(1.0, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(32, 0xff0000, 0xff00, 0xff, 0, 0);
    fps = fps_hermes(1.0, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(32, 0xff, 0xff00, 0xff0000, 0, 0);
    fps = fps_hermes(1.0, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << '\n';

    std::cerr << "zoom 1.90x  ";
    fps = fps_hermes_grey(0.526, 1.0, backbuffer);
    std::cerr << 12*512*fps << "  ";

    src_format = Hermes_FormatNew(16, 0xf800, 0x07e0, 0x001f, 0, 0);
    fps = fps_hermes(0.526, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(24, 0xff0000, 0xff00, 0xff, 0, 0);
    fps = fps_hermes(0.526, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(24, 0xff, 0xff00, 0xff0000, 0, 0);
    fps = fps_hermes(0.526, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(32, 0xff0000, 0xff00, 0xff, 0, 0);
    fps = fps_hermes(0.526, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << "  ";
    src_format = Hermes_FormatNew(32, 0xff, 0xff00, 0xff0000, 0, 0);
    fps = fps_hermes(0.526, 1.0, src_format, backbuffer);
    std::cerr << 12*512*fps << '\n';

    std::cerr << "zoom 0.51x  ";
    fps = fps_hermes_grey(1.0, 0.51, backbuffer);
    std::cerr << .51*0.51*512*512*fps << "  ";

    src_format = Hermes_FormatNew(16, 0xf800, 0x07e0, 0x001f, 0, 0);
    fps = fps_hermes(1.0, 0.51, src_format, backbuffer);
    std::cerr << .51*0.51*512*512*fps << "  ";
    src_format = Hermes_FormatNew(24, 0xff0000, 0xff00, 0xff, 0, 0);
    fps = fps_hermes(1.0, 0.51, src_format, backbuffer);
    std::cerr << .51*0.51*512*512*fps << "  ";
    src_format = Hermes_FormatNew(24, 0xff, 0xff00, 0xff0000, 0, 0);
    fps = fps_hermes(1.0, 0.51, src_format, backbuffer);
    std::cerr << .51*0.51*512*512*fps << "  ";
    src_format = Hermes_FormatNew(32, 0xff0000, 0xff00, 0xff, 0, 0);
    fps = fps_hermes(1.0, 0.51, src_format, backbuffer);
    std::cerr << .51*0.51*512*512*fps << "  ";
    src_format = Hermes_FormatNew(32, 0xff, 0xff00, 0xff0000, 0, 0);
    fps = fps_hermes(1.0, 0.51, src_format, backbuffer);
    std::cerr << .51*0.51*512*512*fps << '\n';
  }
  Hermes_Done();
  }
#endif
  vpl_sleep(5);
  return 0;
}
