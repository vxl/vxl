// \author fsm

#include <vcl_iostream.h>
#include <vul/vul_sprintf.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_pixel.h>

#include <testlib/testlib_test.h>

#if 0 // two unused "static" functions
static
void print_byte(GLubyte w)
{
  for (int i=7; i>=0; --i)
    if ( (w & (0x1 << i)) )
      vcl_cerr << '1';
    else
      vcl_cerr << '0';
}

static
void print_word(GLuint w)
{
  for (int i=31; i>=0; --i)
    if ( (w & (0x1 << i)) )
      vcl_cerr << '1';
    else
      vcl_cerr << '0';
}
#endif // 0

static
void print_binary(char const *fmt, void const *addr, unsigned bytes)
{
  if (fmt)
    vcl_cerr << fmt << " : ";
  GLubyte const *ptr = (GLubyte const*)addr;

  for (int i=bytes; i<4; ++i)
    vcl_cerr << "--------" << ' ';

  for (int i=bytes-1; i>=0; --i)
  {
    GLubyte b = ptr[i];
    for (int j=7; j>=0; --j)
      if ( (b & (0x1 << j)) )
        vcl_cerr << '1';
      else
        vcl_cerr << '0';
    vcl_cerr << ' ';
  }

  vcl_cerr << "| ";

  for (unsigned int i=0; i<bytes; ++i)
  {
    GLubyte b = ptr[i];
    for (int j=7; j>=0; --j)
      if ( (b & (0x1 << j)) )
        vcl_cerr << '1';
      else
        vcl_cerr << '0';
    vcl_cerr << ' ';
  }

  for (unsigned int i=bytes; i<4; ++i)
    vcl_cerr << "--------" << ' ';

  vcl_cerr << vcl_endl;
}

void
test_pixels()
{
#if VXL_LITTLE_ENDIAN
  vcl_cerr << "this machine is little-endian\n";
#endif
#if VXL_BIG_ENDIAN
  vcl_cerr << "this machine is big-endian\n";
#endif

  vcl_cerr << "                        little-endian                         big-endian\n"
           << "               p[3]     p[2]     p[1]     p[0] |     p[0]     p[1]     p[2]     p[3]\n";

  for (int i=0; i<32; ++i)
  {
    const char* buf = vul_sprintf("0x1<<%3d", i);
    GLuint  w = (0x1 << i);
    print_binary(buf, &w, sizeof(w));
  }
  vcl_cerr << "\nr=1, g=0, b=1, a=0\n\n";

  // rgb
  { vgui_pixel_rgb<5,6,5> pix; pix.R=255; pix.G= 0; pix.B=255; print_binary("  rgb565", &pix, sizeof(pix)); }
  { vgui_pixel_rgb<8,8,8> pix; pix.R=255; pix.G= 0; pix.B=255; print_binary("  rgb888", &pix, sizeof(pix)); }
  // bgr
  { vgui_pixel_bgr<5,6,5> pix; pix.R=255; pix.G= 0; pix.B=255; print_binary("  bgr565", &pix, sizeof(pix)); }
  { vgui_pixel_bgr<8,8,8> pix; pix.R=255; pix.G= 0; pix.B=255; print_binary("  bgr888", &pix, sizeof(pix)); }
  // rgba
  { vgui_pixel_rgba<8,8,8,8> pix; pix.R=255; pix.G= 0; pix.B=255; pix.A= 0; print_binary("rgba8888", &pix, sizeof(pix)); }
  // abgr
  { vgui_pixel_abgr<8,8,8,8> pix; pix.R=255; pix.G= 0; pix.B=255; pix.A= 0; print_binary("abgr8888", &pix, sizeof(pix)); }
  // bgra
  { vgui_pixel_bgra<8,8,8,8> pix; pix.R=255; pix.G= 0; pix.B=255; pix.A= 0; print_binary("bgra8888", &pix, sizeof(pix)); }

  TEST( "size of rgb565", sizeof(vgui_pixel_rgb<5,6,5>), 2 );
  TEST( "size of rgb888", sizeof(vgui_pixel_rgb<8,8,8>), 3 );
  TEST( "size of bgr565", sizeof(vgui_pixel_bgr<5,6,5>), 2 );
  TEST( "size of bgr888", sizeof(vgui_pixel_bgr<8,8,8>), 3 );
  TEST( "size of rgba8888", sizeof(vgui_pixel_rgba<8,8,8,8>), 4 );
  TEST( "size of abgr8888", sizeof(vgui_pixel_abgr<8,8,8,8>), 4 );
  TEST( "size of bgra8888", sizeof(vgui_pixel_bgra<8,8,8,8>), 4 );
}

TESTMAIN( test_pixels );
