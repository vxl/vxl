// This is core/vil/vil_pyramid_image_resource.h
#ifndef vil_pyramid_image_resource_h_
#define vil_pyramid_image_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Representation of a pyramid resolution hierarchy
//
// \author J. L. Mundy
// \date 19 March 2006

#include <vcl_vector.h>
#include <vil/vil_image_view_base.h>
#include <vcl_cassert.h>
#include <vil/vil_smart_ptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_blocked_image_resource_sptr.h>

//: Representation of a pyramid resolution hierarchy; mostly pure virtual methods
//
// The pyramid consists of a set of scaled (lower resolution) copies of the base
// image.  A typical case is where the copies are each a factor of two smaller
// than the next lower image in the pyramid. However, there is no intrinsic
// assumption about the scale difference between copies at different levels.
//
// All image positions and sizes are expressed in the coordinate system of
// the base image. The transformation from base image coordinates to one
// of the copy images is given by:  ic = ib*scale, jc = jb*scale. Thus the
// base image is considered to have scale = 1.0.
// The copies all have scale<1.0.

class vil_pyramid_image_resource : public vil_image_resource
{
 public:
  vil_pyramid_image_resource(vcl_vector<vil_image_resource_sptr> const& images);
  virtual ~vil_pyramid_image_resource();

  //: The number of planes (or components) in the image.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const = 0;

  //: The number of pixels in each row.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  virtual unsigned ni() const = 0;

  //: The number of pixels in each column.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  virtual unsigned nj() const = 0;

  //: Pixel Format.
  virtual enum vil_pixel_format pixel_format() const = 0;

  //: Create a read/write view of a copy of this data.
  // Applies only to the base image
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0,
                                                 unsigned n_i,
                                                 unsigned j0,
                                                 unsigned n_j) const
    {return this->get_copy_view(i0, n_i, j0, n_j, 0);}

  //: Put the data in this view back into the base image.
  // Pyramid is readonly.
  // This is essentially (although not formally) a pure virtual function.
  virtual bool put_view(vil_image_view_base const& im, unsigned i0, unsigned j0)
  { return false; }

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const = 0;

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const;

  // === Methods particular to pyramid resource ===

  //: Number of pyramid levels
  virtual unsigned nlevels() const = 0;

  //: Get a partial view from the image from a specified pyramid level
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j,
                                                 unsigned level) const = 0;

  //: Get a complete view from a specified pyramid level.
  // This method needs no specialisation.
  virtual vil_image_view_base_sptr get_copy_view(unsigned level) const
  { return get_copy_view(0, ni(), 0, nj(), level); }

  //: Get a partial view from the image in the pyramid closest to scale.
  // The origin and size parameters are in the coordinate system of the base image.
  // The scale factor is with respect to the base image (base scale = 1.0).
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j,
                                                 const float scale,
                                                 float& actual_scale) const = 0;

  //: Get a complete view from the image in the pyramid closest to the specified scale.
  // The scale factor is with respect to the base image (base scale = 1.0).
  // This method needs no specialisation.
  virtual vil_image_view_base_sptr get_copy_view(const float scale, float& actual_scale) const
  { return get_copy_view(0, ni(), 0, nj(), scale, actual_scale); }

  //: Copy a resource into the pyramid, level is determined by resource scale
  virtual bool put_resource(vil_image_resource_sptr const& resc) = 0;

  //: Get an image resource from the pyramid at the specified level
  virtual vil_image_resource_sptr get_resource(const unsigned level) const = 0;

  //: A utility function to decimate a resource using blocks.
  // The new scale is one half the input resource scale
  static bool
    blocked_decimate(vil_blocked_image_resource_sptr const & brsc,
                     vil_blocked_image_resource_sptr& dec_resc);

  //: Utility for decimating a resource to create a new pyramid level.
  // The new scale is one half the input resource scale. Creates a
  // new resource from filename according to file format
  static vil_image_resource_sptr decimate(vil_image_resource_sptr const& resc,
                                          char const* filename,
                                          char const* format="tiff");

  //: for debug purposes
  virtual void print(const unsigned level) = 0;

 protected:
  //smart pointer
  friend class vil_smart_ptr<vil_pyramid_image_resource>;
  // no default constructor
  vil_pyramid_image_resource();
};

#include <vil/vil_pyramid_image_resource_sptr.h>

#endif // vil_pyramid_image_resource_h_
