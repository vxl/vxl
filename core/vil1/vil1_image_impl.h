// This is ./vxl/vil/vil_image_impl.h
#ifndef vil_image_impl_h_
#define vil_image_impl_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Representation of a generic image
//
// A vil_image_impl is reference counted (see below).
// For a smart-pointer version, use class vil_image.
//
// \author AWF
// \date 17 Feb 2000
//
//\verbatim
//  Modifications
//     000216 AWF Initial version.
//\endverbatim

#include <vcl_cassert.h>
#include <vcl_string.h>

class vil_image;

//: Describes the interpretation of component bits.
// These make no statement about the number of
// bits in the representation.
enum vil_component_format {
  VIL_COMPONENT_FORMAT_UNKNOWN,
  VIL_COMPONENT_FORMAT_UNSIGNED_INT,
  VIL_COMPONENT_FORMAT_SIGNED_INT,
  VIL_COMPONENT_FORMAT_IEEE_FLOAT,
  VIL_COMPONENT_FORMAT_COMPLEX
};

inline
const char* vil_print(vil_component_format f)
{
  switch(f) {
    case VIL_COMPONENT_FORMAT_UNKNOWN: return "VIL_COMPONENT_FORMAT=UNKNOWN";
    case VIL_COMPONENT_FORMAT_UNSIGNED_INT: return "VIL_COMPONENT_FORMAT=unsigned int";
    case VIL_COMPONENT_FORMAT_SIGNED_INT: return "VIL_COMPONENT_FORMAT=signed int";
    case VIL_COMPONENT_FORMAT_IEEE_FLOAT: return "VIL_COMPONENT_FORMAT=IEEE float";
    case VIL_COMPONENT_FORMAT_COMPLEX: return "VIL_COMPONENT_FORMAT=complex";
    default: return "VIL_COMPONENT_FORMAT_INVALID";
  }
}

//:
// Representation of a generic image.
//
// \verbatim
//                        Component   Cell     Pixel      get_section(plane=0,
//                        example     example  example      x0=0,y0=0,w=1,h=1)
//
//  3 x W x H x 1
//     +------+           r           r        r,g,b      r
//     |r     |           g           g
//   +-|      |           b           b
//   |g|      |
// +-| +------+
// |b|      |
// | +------+
// |      |
// +------+
//
// 1 x W x H x 3
// +------------+         r           rgb       rgb       rgb
// |rgb|rgb|    |
// +---+---+    |
// |            |
// |            |
// |            |
// +------------+
// \endverbatim
//
// document relationship between :
// - Component: r, r
// - Cell: r, rgb
// - Pixel : spatial location,
// - Plane

class vil_image_impl {
public:
  vil_image_impl();
  virtual ~vil_image_impl();

  //: Dimensions:  Planes x W x H x Components
  virtual int planes() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int width() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int height() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int components() const = 0;

  //: Format.
  //  A standard RGB RGB RGB image has
  // - components() == 3
  // - bits_per_component() == 8
  // - component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT
  virtual int bits_per_component() const = 0;

  //: Format.
  //  A standard RGB RGB RGB image has
  // - components() == 3
  // - bits_per_component() == 8
  // - component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT
  virtual enum vil_component_format component_format() const = 0;

  //: return the ith plane.
  virtual vil_image get_plane(int ) const;

  //: Copy buffer of this to BUF.
  // The buffer is stored like this for each pixel:
  // component0(plane0,plane1,plane2,...),component1(plane0,plane1,plane2,...),... \n
  //   total size of BUF in bytes should be (bits_per_component * components + 7) / 8
  //   i.e. rounding to the next multiple of 8 bits (only correct if 1 byte = 8 bits)
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const = 0;

  //: Copy plane PLANE of BUF to this.
  // The buffer should look like this for each pixel:
  // component0(plane0,plane1,plane2,...),component1(plane0,plane1,plane2,...),... \n
  // total size of BUF in bytes should be (bits_per_component * components + 7) / 8
  // i.e. rounding to the next multiple of 8 bits (only correct if 1 byte = 8 bits)
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height) = 0;

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return 0; }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const;
  virtual bool set_property(char const* tag, void const* property_value = 0) const;


/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

/* END_MANCHESTER_BINARY_IO_CODE */

private:
  friend class vil_image;
  // You probably should not use a vil_image_impl in a vbl_smart_ptr, so the
  // ref counting methods are called by the unusual up_ref() and down_ref().
  void up_ref() { ++reference_count; }
  void down_ref() {
  assert(reference_count>0);
  if (--reference_count<=0) delete this;
  }
  int reference_count;
};

#define VIL_DISPATCH_AUX(VTYPE, uchar, Template, Args) \
case VTYPE: Template<uchar > Args; break;

#define VIL_DISPATCH_IMAGE_OP(f, Template, Args) \
switch (f) {\
 VIL_DISPATCH_AUX(VIL_UNSIGNED_8, vil_unsigned_8);\
 VIL_DISPATCH_AUX(VIL_SIGNED_8, vil_signed_8); \
 VIL_DISPATCH_AUX(VIL_FLOAT_32, vil_float_32); \
}

#endif // vil_image_impl_h_
