// This is core/vil1/file_formats/vil1_gen.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_gen.h"

#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_cstdio.h>  // sprintf()
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

char const* vil1_gen_format_tag = "gen";

vil1_image_impl* vil1_gen_file_format::make_input_image(vil1_stream* vs)
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
  vcl_cerr << "vil1_gen_file_format: s= [" << s << "]\n";

  bool ok = (s[0] == 'g' &&
             s[1] == 'e' &&
             s[2] == 'n' &&
             s[3] == ':');

  if (!ok)
    return 0;

  vcl_cerr << "vil1_gen_file_format: s= [" << s << "]\n";

  return new vil1_gen_generic_image(s);
}

char const* vil1_gen_file_format::tag() const
{
  return vil1_gen_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

char const* vil1_gen_generic_image::file_format() const
{
  return vil1_gen_format_tag;
}

vil1_gen_generic_image::vil1_gen_generic_image(vcl_string const& /*s*/,
                                               int /*planes*/,
                                               int /*width*/,
                                               int /*height*/,
                                               int /*components*/,
                                               int /*bits_per_component*/,
                                               vil1_component_format /*format*/)
{
  vcl_abort();
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

void vil1_gen_generic_image::init(vcl_string const& s)
{
  char const* p = s.c_str();
  // 1. Skip over "gen:"
  p += 4;
  // 2. Get size
  width_ = -1;
  height_ = -1;
  vcl_sscanf(p, "%dx%d:", &width_, &height_);
  if (height_ == -1) {
    vcl_cerr << "vil1_gen_generic_image: bad height, should be gen:WxH:\n";
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

  vcl_cerr << "vil1_gen_generic_image: type = ["<<type<<"]\n";

  if (type == "grey" || type == "gray")
  {
    if (*p == ',') {
      ++p;
      params_[0] = read_int(&p);
    }
    else
      params_[0] = 128;
    components_ = 1;
    bits_per_component_ = 8;
    type_ = vil1_gen_gray;

    vcl_cerr << "vil1_gen_generic_image: p0 = ["<<params_[0]<<"]\n";
  }
  else if (type == "rgb")
  {
    if (*p == ',') {
      ++p;
      params_[0] = read_int(&p);
      ++p;
      params_[1] = read_int(&p);
      ++p;
      params_[2] = read_int(&p);
    }
    else
      params_[0] = 128;
    components_ = 3;
    bits_per_component_ = 8;
    type_ = vil1_gen_rgb;

    vcl_cerr << "vil1_gen_generic_image: p0 = [" << params_[0] << "], p1 = ["
             << params_[1] << "], p2 = [" << params_[2] << "]\n";
  }
  else
    assert(!"type must be one of grey, gray or rgb");
}

bool vil1_gen_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

bool vil1_gen_generic_image::get_section(void* buf, int /*x0*/, int /*y0*/, int xs, int ys) const
{
  // A constant (generic) image pixel value is independent of (x0,y0)
  if (type_ == vil1_gen_gray) {
    vcl_memset(buf, params_[0], xs*ys);
    return true;
  }
  else if (type_ == vil1_gen_rgb) {
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
  else
    return false;
}

bool vil1_gen_generic_image::put_section(void const* /*buf*/, int /*x0*/, int /*y0*/, int /*xs*/, int /*ys*/)
{
  vcl_abort();
  return false;
}

vil1_image vil1_gen_generic_image::get_plane(unsigned int plane) const
{
  assert(plane == 0);
  return const_cast<vil1_gen_generic_image*>(this);
}
