/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_pixel.h>

void print_byte(GLubyte w) {
  for (int i=7; i>=0; --i)
    if ( (w & (0x1 << i)) )
      vcl_cerr << '1';
    else
      vcl_cerr << '0';
}

#if 0
void print_word(GLuint w) {
  for (int i=31; i>=0; --i)
    if ( (w & (0x1 << i)) )
      vcl_cerr << '1';
    else
      vcl_cerr << '0';
}
#endif

void print_binary(char const *fmt, void const *addr, unsigned bytes) {
  if (fmt)
    vcl_cerr << fmt << " : ";
  GLubyte const *ptr = (GLubyte const*)addr;

  for (int i=bytes; i<4; ++i)
    vcl_cerr << "--------" << ' ';

  for (int i=bytes-1; i>=0; --i) {
    GLubyte b = ptr[i];
    for (int j=7; j>=0; --j)
      if ( (b & (0x1 << j)) )
        vcl_cerr << '1';
      else
        vcl_cerr << '0';
    vcl_cerr << ' ';
  }

  vcl_cerr << "| ";

  for (unsigned int i=0; i<bytes; ++i) {
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

int main(int, char **) {
#if VXL_LITTLE_ENDIAN
  vcl_cerr << "this machine is little-endian" << vcl_endl;
#endif
#if VXL_BIG_ENDIAN
  vcl_cerr << "this machine is big-endian" << vcl_endl;
#endif

  vcl_cerr << "                        little-endian                         big-endian" << vcl_endl;
  vcl_cerr << "               p[3]     p[2]     p[1]     p[0] |     p[0]     p[1]     p[2]     p[3]" << vcl_endl;

  for (int i=0; i<32; ++i) {
    char buf[64]; vcl_sprintf(buf, "0x1<<%3d", i);
    GLuint  w = (0x1 << i);
    print_binary(buf, &w, sizeof(w));
  }
  vcl_cerr << vcl_endl
       << "r=1, g=0, b=1, a=0" << vcl_endl
       << vcl_endl;

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

  return 0;
}
