#ifndef vil_image_h_
#define vil_image_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_iosfwd.h>

#include <vil/vil_image_impl.h>

#if 0
//: Convenient descriptions of format combinations.
// Combinations of component format and bits and planes and components
enum vil_pixel_format {
  VIL_PIXEL_FORMAT_UNKNOWN,
  VIL_BYTE,       /*!< 1 x W x H x 1 of UINT x 8*/
  VIL_RGB_BYTE,   /*!< 1 x W x H x 3 of UINT x 8*/
  VIL_UINT16,     /*!< 1 x W x H x 1 of UINT x 16*/
  VIL_UINT32,     /*!< 1 x W x H x 1 of UINT x 32*/
  VIL_FLOAT,      /*!< 1 x W x H x 1 of IEEE_FLOAT x 32*/
  VIL_DOUBLE,     /*!< 1 x W x H x 1 of IEEE_DOUBLE x 64*/
  VIL_COMPLEX    /*!< 1 x W x H x 1 of COMPLEX x 64*/
};
#endif

class vil_image;

#define VIL_IMAGE_USE_SAFETY_NET (1)

//: A reference-counted image object.
// All operations are delegated to the vil_image_impl object,
// which uses class inheritance to make various file images etc.
// For fuller documentation on any method, see vil_image_impl
class vil_image {
public:
  //: delegation macro for consistency, not convenience.
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
  
  //: Extra property information
  bool get_property(char const *tag, void *property_value_out = 0) 
    { vil_image_delegate(get_property, (tag, property_value_out), false); }
  
  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  char const *file_format() const { vil_image_delegate(file_format, (), "(null)"); }

#undef vil_image_delegate
  // -------------------- convenience --------------------

#if 0
  //: Convenience method.
  //  converts (bits_per_component x component_format)
  // to one of the `vil_pixel_format's. 
  // A standard RGB RGB RGB image has pixel_type() == VIL_RGB_BYTE
  enum vil_pixel_format pixel_type() const;
#endif

  //: return size in bytes.
  int get_size_bytes() const;
    
#if 0
  //: \brief Return the image interpreted as rgb bytes.
  bool get_section_rgb_byte(void *buf, int x0, int y0, int width, int height) const;

  //: Return the image interpreted as floats.
  bool get_section_float(void *buf, int x0, int y0, int width, int height) const;

  //: Return the image interpreted as bytes.
  bool get_section_byte(void *buf, int x0, int y0, int width, int height) const;
#endif
  //: Print a 1-line summary of contents
  ostream& print(ostream&) const;
  
  //--------------------------------- Smart pointer behaviour ---------------------------------

  vil_image(vil_image_impl *p = 0) : ptr(p) {
    if (ptr)
      ptr->up_ref();
#if VIL_IMAGE_USE_SAFETY_NET
    xx_register(this);
#endif
  }

  vil_image(vil_image const& that) : ptr(that.ptr) {
    if (ptr)
      ptr->up_ref();
#if VIL_IMAGE_USE_SAFETY_NET
    xx_register(this);
#endif
  }

  ~vil_image() {
#if VIL_IMAGE_USE_SAFETY_NET
    xx_unregister(this);
#endif
    if (ptr)
      ptr->down_ref();
    ptr = 0; // don't dangle
  }

  vil_image& operator=(vil_image const &that) {
    if (that.ptr) // first ref that.ptr in case ptr == that.ptr
      that.ptr->up_ref();
#if VIL_IMAGE_USE_SAFETY_NET
    // fsm: set 'ptr' to zero in *this before calling down_ref() on
    // *ptr. else a vil_image_impl will warn about it still being
    // reffed.
    vil_image_impl *old = ptr;
    ptr = 0;
    if (old)
      old->down_ref();
    ptr = that.ptr;
#else
    if (ptr)
      ptr->down_ref();
    ptr = that.ptr;
#endif
    return *this;
  }

  vil_image& operator=(vil_image_impl *p) {
    if (ptr)
      ptr->down_ref();
    ptr = p;
    return *this;
  }

  // use "sptr.impl()" to get a pointer to the impl object.
  vil_image_impl *impl() const {
    return ptr;
  }
  
  // comparison
  bool operator==(vil_image const &that) const {
    return ptr == that.ptr;
  }

  // conversion to bool
  operator bool () const {
    return ptr != 0;
  }
  
protected:
  vil_image_impl *ptr;

#if VIL_IMAGE_USE_SAFETY_NET
  friend class vil_image_impl;
  static void xx_register(vil_image const *);
  static void xx_unregister(vil_image const *);
  static bool is_reffed(vil_image_impl const *);
#endif
};

//: Print a 1-line summary of contents
inline
ostream& operator<<(ostream& s, vil_image const& i) {
  return i.print(s);
}

#endif
