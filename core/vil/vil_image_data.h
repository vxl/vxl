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
#include <vil2/vil2_smart_ptr.h>
#include <vil2/vil2_image_view_base.h>
#include <vil2/vil2_pixel_format.h>

class vil2_image_view_base;

//:
// Abstract representation of an image source or image destination.
// Most references to vil2_image_data objects should usually be done
// through smart pointers - vil2_image_data_sptr;
// All image data is presumed to be arranged in planes, not components.

class vil2_image_data
{
 public:
  vil2_image_data();
  virtual ~vil2_image_data();

  //: Dimensions:  Planes x ni x nj.
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const = 0;
  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each row.
  virtual unsigned ni() const = 0;
  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each column.
  virtual unsigned nj() const = 0;

  //: Format.
  //  A standard RGB RGB RGB of chars image has
  // pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE
  virtual enum vil2_pixel_format pixel_format() const = 0;

  //: Create a read/write view of the data.
  // Modifying this view might modify the actual data.
  // If you want to modify this data in place, call put_view after you done, and
  // it should work efficiently. This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base_sptr get_view(unsigned x0, unsigned y0,
                                             unsigned nx, unsigned ny) const
  { return get_copy_view (x0, y0, nx, ny); }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size. Caller is responsible for
  // deleting the view. This function will always return a
  // multi-plane scalar-pixel view of the data.
  virtual vil2_image_view_base_sptr get_copy_view(unsigned x0, unsigned y0,
                                                  unsigned nx, unsigned ny) const = 0;

  //: Put the data in this view back into the image source.
  // The view must be of scalar components. Use vil2_view_as_planes
  // to convert it if this is not the case.
  virtual bool put_view(const vil2_image_view_base& im, unsigned x0, unsigned y0) = 0;

  //: Check that a view will fit into the data at the given offset.
  // This includes checking that the pixel type is scalar.
  virtual bool view_fits(const vil2_image_view_base& im, unsigned x0, unsigned y0);

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

typedef vil2_smart_ptr<vil2_image_data> vil2_image_data_sptr;

#endif // vil2_image_data_h_
