// This is core/vil/vil_image_resource.h
#ifndef vil_image_resource_h_
#define vil_image_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Representation of a generic image source or destination.
//
// \author Ian Scott
// \date 20 Sep 2002

#include <vil/vil_image_view_base.h>
#include <vcl_cassert.h>
#include <vil/vil_smart_ptr.h>
#include <vil/vil_pixel_format.h>

#include <vil/vil_image_resource_sptr.h>

//:
// Abstract representation of an image source or image destination.
// Most references to vil_image_resource objects should usually be done
// through smart pointers - vil_image_resource_sptr;
//
// All image data is presumed to be in planes, not components. This
// does not say whether the data is stored on disk or in memory
// as RGBRGBRGB.. or RRR..GGG..BBB.., just that the interface will
// always tell you that it has a multi-plane single-component view.
class vil_image_resource
{
 public:
  vil_image_resource();
  virtual ~vil_image_resource();

  //: Dimensions:  Planes x ni x nj.
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const = 0;
  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each row.
  virtual unsigned ni() const = 0;
  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each column.
  virtual unsigned nj() const = 0;

  //: Pixel Format.
  //  A standard RGB RGB RGB of chars image has
  // pixel_format() == VIL_PIXEL_FORMAT_BYTE
  virtual enum vil_pixel_format pixel_format() const = 0;

  //: Create a read/write view of the data.
  // Modifying this view might modify the actual data.
  // If you want to modify this data in place, call put_view after you done, and
  // it should work efficiently. This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  //
  // If you want to fill an existing view (e.g. a window onto some other image),
  // then use
  // \verbatim
  // vil_reformat(data->get_view(..), window);
  //\endverbatim
  virtual vil_image_view_base_sptr get_view(unsigned i0, unsigned n_i,
                                            unsigned j0, unsigned n_j) const
  { return get_copy_view (i0, n_i, j0, n_j); }

  //: Create a read/write view of all the data.
  vil_image_view_base_sptr get_view() const
  { return get_view (0, ni(), 0, nj()); }

  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j) const = 0;

  //: Create a read/write view of a copy of all the data.
  vil_image_view_base_sptr get_copy_view() const
  { return get_copy_view (0, ni(), 0, nj()); }

  //: Put the data in this view back into the image source.
  // The view must be of scalar components. Assign your
  // view to a scalar-component view if this is not the case.
  // \return false if failed, because e.g. resource is read-only,
  // format of view is not correct (if it is a compound pixel type, try
  // assigning it to a multi-plane scalar pixel view.)
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) = 0;

  //: Put the data in this view back into the image source at the origin
  bool put_view(const vil_image_view_base& im)
  { return put_view(im, 0, 0); }

  //: Check that a view will fit into the data at the given offset.
  // This includes checking that the pixel type is scalar.
  virtual bool view_fits(const vil_image_view_base& im, unsigned i0, unsigned j0);

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return 0; }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const =0;

 private:
  // You probably should not use a vil_image_resource in a vbl_smart_ptr, so the
  // ref functions are private
  friend class vil_smart_ptr<vil_image_resource>;
  void ref() { ++reference_count_; }
  void unref() {
    assert(reference_count_>0);
    if (--reference_count_<=0) delete this;}
  int reference_count_;
};

#endif // vil_image_resource_h_
