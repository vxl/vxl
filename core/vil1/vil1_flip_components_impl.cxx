// This is core/vil1/vil1_flip_components_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_flip_components_impl.h"
#include <vcl_cstring.h> // for memcpy

vil1_image vil1_flip_components_impl::get_plane(unsigned int p) const
{
  return new vil1_flip_components_impl(base.get_plane(p));
}

static void do_swap(char* buf, int nr_bytes, int nr_swaps)
{
  if (nr_swaps <= 0) return;
  while (nr_swaps--) {
    for (int i=0; i<nr_bytes; ++i) {
      char t = buf[i]; buf[i] = buf[i+2*nr_bytes]; buf[i+2*nr_bytes] = t; }
    buf += 3*nr_bytes;
  }
}

bool vil1_flip_components_impl::get_section(void *buf, int x0, int y0, int w, int h) const
{
  if (!base.get_section(buf, x0, y0, w, h))
    return false;
  if (base.components() != 3) // no swapping necessary since not 3 colour cells
    return true;
  int bpc = base.bits_per_component();
  if (bpc&7) return false; // TODO: currently this flipping only works when bpc is a multiple of 8
  do_swap(static_cast<char*>(buf), bpc/8, w*h);
  return true;
}

bool vil1_flip_components_impl::put_section(void const *buf, int x0, int y0, int w, int h)
{
  if (base.components() != 3) // no swapping necessary since not 3 colour cells
    return base.put_section(buf, x0, y0, w, h);
  int bpc = base.bits_per_component();
  if (bpc&7) return false; // TODO: currently this flipping only works when bpc is a multiple of 8
  char* b = new char[bpc/8 * w*h * 3];
  vcl_memcpy(b, buf, bpc/8 * w*h * 3);
  do_swap(b, bpc/8, w*h);
  bool r = base.put_section(b, x0, y0, w, h);
  delete[] b;
  return r;
}

//: Return the name of the class
vcl_string vil1_flip_components_impl::is_a() const
{
  static const vcl_string class_name_="vil1_flip_components_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_flip_components_impl::is_class(vcl_string const& s) const
{
  return s==vil1_flip_components_impl::is_a() || vil1_image_impl::is_class(s);
}
