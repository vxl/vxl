// This is core/vil/file_formats/vil_j2k_pyramid_image_resource.h
#ifndef vil_j2k_pyramid_image_resource_h_
#define vil_j2k_pyramid_image_resource_h_
//:
// \file
// \brief Representation of a pyramid resolution hierarchy based on the j2k_image
//
// \author J. L. Mundy
// \date April 22, 2009
// Do not remove the following notice
// Approved for public Release, distribution unlimited
// DISTAR Case 14074
//
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_pyramid_image_resource.h>
#include <vil/file_formats/vil_j2k_image.h>

class vil_j2k_pyramid_image_resource : public vil_pyramid_image_resource
{
 public:
  vil_j2k_pyramid_image_resource(vil_image_resource_sptr const& j2k);
  virtual ~vil_j2k_pyramid_image_resource(){}

  //: The number of planes (or components) in the image.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const;

  //: The number of pixels in each row.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  virtual unsigned ni() const;

  //: The number of pixels in each column.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  virtual unsigned nj() const;

  //: Pixel Format.
  virtual enum vil_pixel_format pixel_format() const;

  //: Put the data in this view back into the base image.
  // Pyramid is readonly.
  // This is essentially (although not formally) a pure virtual function.
  virtual bool put_view(vil_image_view_base const& /*im*/, unsigned /*i0*/, unsigned /*j0*/)
  { return false; }

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const;

  // === Methods particular to pyramid resource ===

  //: Number of pyramid levels.
  // In principle, the number of levels is
  // undefined since the j2k decoder can decompress to any window size.
  // A reasonable strategy is to define the number of levels so that
  // the lowest scale produces a smallest image dimension of 1K pixels
  virtual unsigned nlevels() const;

  //: Get a partial view from the image from a specified pyramid level
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j,
                                                 unsigned level) const;

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
                                                 float& actual_scale) const;

  //: Get a complete view from the image in the pyramid closest to the specified scale.
  // The scale factor is with respect to the base image (base scale = 1.0).
  // This method needs no specialisation.
  virtual vil_image_view_base_sptr get_copy_view(const float scale, float& actual_scale) const
  { return get_copy_view(0, ni(), 0, nj(), scale, actual_scale); }

  //: Copy a resource into the pyramid, level is determined by resource scale
  virtual bool put_resource(vil_image_resource_sptr const& resc)
    {return false;}

  //: Get an image resource from the pyramid at the specified level
  virtual vil_image_resource_sptr get_resource(const unsigned level) const;

  //: for debug purposes
  virtual void print(const unsigned level);

 protected:
  vil_j2k_pyramid_image_resource();
  vil_image_resource_sptr j2k_sptr_;
  vil_j2k_image* ptr_;
};


#endif // vil_j2k_pyramid_image_resource_h_
