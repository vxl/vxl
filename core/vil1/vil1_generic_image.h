#ifndef vil_generic_image_h_
#define vil_generic_image_h_

//: 
// \file vil_generic_image.h
// \author AWF
// \date 17 Feb 2000
// \bug No modifications yet
// 
// Representation of a generic image. A vil_generic_image is
// reference counted and starts life with a reference count of 0.
// If the reference count is decrease to 0 again, the object is
// deleted. Use vil_image_ref for a smart-pointer version of
// vil_generic_image.


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
  VIL_COMPLEX,    /*!< 1 x W x H x 1 of COMPLEX x 64*/
};

//:
// Representation of a generic image.
//
// \verbatim
// 
//                        Component   Cell     Pixel   	 get_section(plane=0,x0=0,y0=0,w=1,h=1)
//                        example     example  example
// 
//  3 x W x H x 1
//     +------+	          r           r	       r,g,b	 r
//     |r     |		  g	      g
//   +-|      |		  b	      b
//   |g|      |
// +-| +------+
// |b|      |
// | +------+
// |      |
// +------+
// 
// 1 x W x H x 3
// +------------+	  r 	      rgb     rgb	 rgb
// |rgb|rgb|    |
// +---+---+    |
// |            |
// |            |
// |            |
// +------------+
// 
// document relationship between :
// Component: r, r
// Cell: r, rgb
// Pixel : spatial location,
// Plane
// \endverbatim
class vil_generic_image {
public:
  vil_generic_image() : reference_count(0) { }
  virtual ~vil_generic_image();

  //: Dimensions:  Planes x W x H x Components
  virtual int planes() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int width() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int height() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int components() const = 0;

  //: Format.
  // \verbatim
  //   A standard RGB RGB RGB image has
  //   components() == 3
  //   bits_per_component() == 8
  //   component_format() == VIL_UNSIGNED_INT
  //   \endverbatim
  virtual int bits_per_component() const = 0;
  //: Format.
  // \verbatim
  //   A standard RGB RGB RGB image has
  //   components() == 3
  //   bits_per_component() == 8
  //   component_format() == VIL_UNSIGNED_INT
  //   \endverbatim
  virtual enum vil_component_format component_format() const = 0;

  //: return the ith plane as a vil_generic_image. do not delete the returned object.
  virtual vil_generic_image* get_plane(int ) const;

  //: Copy buffer of this to BUF.
  // The buffer is stored like this for each pixel: 
  // component0(plane0,plane1,plane2,...),component1(plane0,plane1,plane2,...),...
  // \verbatim
  //   total size of BUF in bytes should be (bits_per_component * components + 7) / 8
  //   i.e. rounding to the next multiple of 8 bits (only correct if 1 byte = 8 bits)
  // \endverbatim
  virtual bool do_get_section(void* buf, int x0, int y0, int width, int height) const = 0;
  //: Copy plane PLANE of BUF to this.
  // The buffer should look like this for each pixel: 
  // component0(plane0,plane1,plane2,...),component1(plane0,plane1,plane2,...),...
  // \verbatim
  // total size of BUF in bytes should be (bits_per_component * components + 7) / 8
  // i.e. rounding to the next multiple of 8 bits (only correct if 1 byte = 8 bits)
  // \endverbatim
  virtual bool do_put_section(void const* buf, int x0, int y0, int width, int height) = 0;

  //: \brief Return the image interpreted as rgb bytes.
  virtual bool get_section_rgb_byte(void* buf, int x0, int y0, int width, int height) const;
  //: Return the image interpreted as floats.
  virtual bool get_section_float(void* buf, int x0, int y0, int width, int height) const;
  //: Return the image interpreted as bytes.
  virtual bool get_section_byte(void* buf, int x0, int y0, int width, int height) const;

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return 0; }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value_out = 0) const;

  //------------------------------ convenience ------------------------------

  //: Convenience method.
  //  converts (bits_per_component x component_format)
  // to one of the `vil_pixel_format's. 
  // A standard RGB RGB RGB image has pixel_type() == VIL_RGB_BYTE
  virtual enum vil_pixel_format pixel_type() const;

  //: Copy from image to buf
  //  Return entire size in bytes.
  virtual int get_size_bytes() const { return planes() * width() * height() * components() * bits_per_component() / 8; }

  //: Copy from image to buf
  bool get_section(void* buf, int x0, int y0, int width, int height) const {
    return do_get_section(buf, x0, y0, width, height);
  }
  //: Copy from buf to image
  bool put_section(void const* buf, int x0, int y0, int width, int height) {
    return do_put_section(buf, x0, y0, width, height);
  }

  //: Methods to support reference counting.
  void ref() { ++reference_count; }
  void unref() { if (--reference_count<=0) delete this; }
private:
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

#endif
