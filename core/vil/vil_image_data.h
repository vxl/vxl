// This is mul/vil2/vil2_image_data.h
#ifndef vil2_image_data_h_
#define vil2_image_data_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Representation of a generic image source or destination.
//
// \author Ian Scott
// \date 20 Sep 2002


#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vil/vil_image.h>
#include <vil2/vil2_smart_ptr.h>


class vil2_image_view_base;

//:
// Abstract representation of an image source or image destination.
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

class vil2_image_data
{
 public:
  vil2_image_data();
  virtual ~vil2_image_data();

  //: Dimensions:  Planes x W x H x Components
  virtual int planes() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int width() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int height() const = 0;
  //: Dimensions:  Planes x W x H x Components
  virtual int components() const = 0;

  //: Number of bits per component.
  // Size (in bits) for the smallest entity of the image.
  virtual int bits_per_component() const = 0;

  //: Format.
  //  A standard RGB RGB RGB image has
  // - components() == 3
  // - bits_per_component() == 8
  // - component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT
  // Use vil_print(fmt) to return a string description of the format fmt.
  virtual enum vil_component_format component_format() const = 0;

  //: Create a new view which is compatible with the underlying image type.
  // \param planewise, Do you want the data provided in planes (or components)?
  virtual vil2_image_view_base* new_view(bool planewise=true);

  //: Create a read/write view of the data.
  // Modifying this view might modify the actual data.
  // If you want to modify this data in place, call put_view after you done, and 
  // it should work efficiently.
  virtual bool get_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned width, unsigned height) const = 0;

  //: Create a read/write view of the data.
  // Modifying this view might modify the actual data.
  // If you want to modify this data in place, call put_view after you done, and 
  // it should work efficiently.
  virtual bool get_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned plane0, unsigned width, unsigned height, unsigned nplanes) const = 0;

  //: Create a read/write view of a copy of this data.
  virtual bool get_copy_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned width, unsigned height) const = 0;

  //: Create a read/write view of a copy of this data.
  virtual bool get_copy_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned plane0, unsigned width, unsigned height, unsigned nplanes) const = 0;

  //: Put the data in this view back into the image source.
  virtual bool put_view(vil2_image_view_base* im, unsigned x0, unsigned y0, unsigned plane0 = 0) = 0;

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return 0; }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const =0;


  //: Return the name of the class;
  virtual vcl_string is_a() const ;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

 private:
  // You probably should not use a vil2_image_data in a vbl_smart_ptr, so the
  // ref functions are private
  friend class vil2_smart_ptr<vil2_image_data>;
  void ref() { ++reference_count_; }
  void unref() {
    assert(reference_count_>0);
    if (--reference_count_<=0) delete this;}
  int reference_count_;
};


#endif // vil2_image_data_h_
