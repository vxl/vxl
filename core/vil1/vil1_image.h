// This is vxl/vil/vil_image.h
#ifndef vil_image_h_
#define vil_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A reference-counted image object.
// \author fsm

#include <vcl_iosfwd.h>
#include <vil/vil_image_impl.h>

//: A smart pointer to an actual image.
// All operations are delegated to the vil_image_impl object,
// which uses class inheritance to make various file images etc.
// For fuller documentation on any method, see vil_image_impl
//
// Imagine that vil_image is a class derived from
// vbl_smart_ptr<vil_image_impl>, but with some extra convenience
// methods.  These methods might traditionally be attached to the abstract
// base class vil_image_impl, but this avoids cluttering that interface.
//
// You should not derive from vil_image to make a new image type.
// Derive from vil_image_impl instead.

class vil_image
{
 public:
// use this delegation macro for consistency, not convenience.
#define vil_image_delegate(m, args, default) { return ptr ? ptr->m args : default; }

  //: Dimensions:  Planes x W x H x Components
  int planes() const { vil_image_delegate(planes, (), 0); }

  //: Dimensions:  Planes x W x H x Components
  int width() const { vil_image_delegate(width, (), 0); }

  //: Dimensions:  Planes x W x H x Components
  int height() const { vil_image_delegate(height, (), 0); }

  //: Dimensions:  Planes x W x H x Components
  int components() const { vil_image_delegate(components, (), 0); }

  //: Format.
  int bits_per_component() const { vil_image_delegate(bits_per_component, (), 0); }

  //: Format.
  enum vil_component_format component_format() const
    { vil_image_delegate(component_format, (), VIL_COMPONENT_FORMAT_UNKNOWN);  }

  //: return the ith plane.
  vil_image get_plane(int p) const { vil_image_delegate(get_plane, (p), vil_image()); }

  //: Copy from image to buf
  bool get_section(void *buf, int x0, int y0, int width, int height) const
    { vil_image_delegate(get_section, (buf, x0, y0, width, height), false); }

  //: Copy from buf to image
  bool put_section(void const *buf, int x0, int y0, int width, int height)
    { vil_image_delegate(put_section, (buf, x0, y0, width, height), false); }

  //: Getting property information
  bool get_property(char const *tag, void *property_value = 0) const
    { vil_image_delegate(get_property, (tag, property_value), false); }

  //: Setting property information
  bool set_property(char const *tag, void const *property_value = 0)
    { vil_image_delegate(set_property, (tag, property_value), false); }

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  char const *file_format() const { vil_image_delegate(file_format, (), "(null)"); }

#undef vil_image_delegate
  // -------------------- convenience --------------------

  //: Number of rows
  int rows() const { return height(); }
  //: Number of columns
  int cols() const { return width(); }

  //: return size in bytes.
  int get_size_bytes() const;

  //: Print a 1-line summary of contents
  vcl_ostream& print(vcl_ostream&) const;

  //------------ smart-pointer logic --------

  vil_image(vil_image_impl *p = 0) : ptr(p)
  {
    if (ptr)
      ptr->up_ref();
  }

  vil_image(vil_image const& that) : ptr(that.ptr) {
    if (ptr)
      ptr->up_ref();
  }

  //: Destructor
  ~vil_image() {
    if (ptr)
      ptr->down_ref();
    ptr = 0; // don't dangle
  }

  vil_image& operator=(vil_image const &that) {
    if (ptr != that.ptr) {
      if (that.ptr)
        that.ptr->up_ref();
      if (ptr)
        ptr->down_ref();
      ptr = that.ptr;
    }
    return *this;
  }

  vil_image& operator=(vil_image_impl *p) {
    if (ptr) 
      ptr->down_ref();
    ptr = p;
    if (ptr)
      ptr->up_ref();
    return *this;
  }

  //: equality means equality of implementation, not pixels.
  bool operator==(vil_image const &that) const {
    return ptr == that.ptr;
  }

  //: needed for sorted containers of images.
  bool operator< (vil_image const &that) const {
    return ptr <  that.ptr;
  }

  //: conversion to bool
  operator bool () const {
    return ptr != 0;
  }

  //: use "sptr.impl()" to get a pointer to the impl object.
  vil_image_impl *impl() const {
    return ptr;
  }

 protected:
  vil_image_impl *ptr;
};

//: Print a 1-line summary of contents
inline
vcl_ostream& operator<<(vcl_ostream& s, vil_image const& i) {
  return i.print(s);
}

#endif // vil_image_h_
