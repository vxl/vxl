// This is vxl/vil/file_formats/vil_gen.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil_gen.h"

#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_cstdio.h>  // sprintf()
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vil/vil_property.h>

char const* vil_gen_format_tag = "gen";

vil_image_impl* vil_gen_file_format::make_input_image(vil_stream* vs)
{
  // Attempt to read header
  vcl_string s;
  for (;;) {
    char buf;
    if (vs->read(&buf, 1L) == 0L)
      return 0;
    if (buf == 0)
      break;
    s += buf;
  }
  vcl_cerr << "vil_gen_file_format: s= [" << s << "]\n";

  bool ok = (s[0] == 'g' &&
             s[1] == 'e' &&
             s[2] == 'n' &&
             s[3] == ':');

  if (!ok)
    return 0;

  vcl_cerr << "vil_gen_file_format: s= [" << s << "]\n";

  return new vil_gen_generic_image(s);
}

char const* vil_gen_file_format::tag() const
{
  return vil_gen_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_gen_generic_image::vil_gen_generic_image(vcl_string const& s)
{
  init(s);
}

char const* vil_gen_generic_image::file_format() const
{
  return vil_gen_format_tag;
}

vil_gen_generic_image::vil_gen_generic_image(vcl_string const& s,
                                             int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil_component_format format)
{
  vcl_abort();
}

vil_gen_generic_image::~vil_gen_generic_image()
{
}

static int read_int(char const** p_inout)
{
  int val = 0;
  char const* p = *p_inout;
  while (*p >= '0' && *p <= '9') {
    int d = *p - '0';
    val = val * 10 + d;
    ++p;
  }
  *p_inout = p;
  return val;
}

void vil_gen_generic_image::init(vcl_string const& s)
{
  char const* p = s.c_str();
  // 1. Skip over "gen:"
  p += 4;
  // 2. Get size
  width_ = -1;
  height_ = -1;
  vcl_sscanf(p, "%dx%d:", &width_, &height_);
  if (height_ == -1) {
    vcl_cerr << "vil_gen_generic_image: bad height, should be gen:WxH:\n";
    width_ = 0;
    height_ = 0;
    return;
  }
  // Skip to colon
  while (*p != ':' && *p != 0)
    ++p;

  assert(*p != 0);

  // Skip colon;
  ++p;

  // Read type
  vcl_string type;
  while (*p != 0 && *p != ',') {
    type += *p;
    ++p;
  }

  vcl_cerr << "vil_gen_generic_image: type = ["<<type<<"]\n";

  if (type == "grey" || type == "gray") {
    if (*p == ',') {
      ++p;
      params_[0] = read_int(&p);
    }
    else
      params_[0] = 128;
    components_ = 1;
    bits_per_component_ = 8;
    type_ = vil_gen_gray;

    vcl_cerr << "vil_gen_generic_image: p0 = ["<<params_[0]<<"]\n";
  } else {
    vcl_abort();
  }
}

bool vil_gen_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

bool vil_gen_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if (type_ == vil_gen_gray) {
    vcl_memset(buf, xs*ys, int(params_[0]));
    return true;
  }
  if (type_ == vil_gen_rgb) {
    int n = xs*ys;
    unsigned char* p = (unsigned char*)buf;
    unsigned char r = (unsigned char)(params_[0]);
    unsigned char g = (unsigned char)(params_[1]);
    unsigned char b = (unsigned char)(params_[2]);
    while (n--) {
      *p++ = r;
      *p++ = g;
      *p++ = b;
    }
    return true;
  }
  return false;
}

bool vil_gen_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  vcl_abort();
  return false;
}

vil_image vil_gen_generic_image::get_plane(int plane) const
{
  assert(plane == 0);
  return const_cast<vil_gen_generic_image*>(this);
}
