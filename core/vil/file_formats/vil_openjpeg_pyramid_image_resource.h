// This is core/vil/file_formats/vil_openjpeg_pyramid_image_resource.h
#ifndef vil_openjpeg_pyramid_image_resource_h_
#define vil_openjpeg_pyramid_image_resource_h_
//:
// \file
// \brief Representation of a pyramid resolution hierarchy based on the openjpeg_image
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
#include <vil/file_formats/vil_openjpeg.h>

class vil_openjpeg_pyramid_image_resource : public vil_pyramid_image_resource
{
 public:
  vil_openjpeg_pyramid_image_resource(vil_image_resource_sptr const& openjpeg);
  ~vil_openjpeg_pyramid_image_resource() override = default;

  //: The number of planes (or components) in the image.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  // This concept is treated as a synonym to components.
  unsigned nplanes() const override;

  //: The number of pixels in each row.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  unsigned ni() const override;

  //: The number of pixels in each column.
  // Dimensions:  Planes x ni x nj.
  // This method refers to the base (max resolution) image
  unsigned nj() const override;

  //: Pixel Format.
  enum vil_pixel_format pixel_format() const override;

  //: Put the data in this view back into the base image.
  // Pyramid is readonly.
  // This is essentially (although not formally) a pure virtual function.
  bool put_view(vil_image_view_base const& /*im*/, unsigned /*i0*/, unsigned /*j0*/) override
  { return false; }

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  char const* file_format() const override;

  // === Methods particular to pyramid resource ===

  //: Number of pyramid levels.
  unsigned nlevels() const override;

  //: Get a partial view from the image from a specified pyramid level
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj,
                                                 unsigned level) const override;

  //: Get a complete view from a specified pyramid level.
  // This method needs no specialization.
  vil_image_view_base_sptr get_copy_view(unsigned level) const override
  { return get_copy_view(0, ni(), 0, nj(), level); }

  //: Get a partial view from the image in the pyramid closest to scale.
  // The origin and size parameters are in the coordinate system of the base image.
  // The scale factor is with respect to the base image (base scale = 1.0).
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj,
                                                 const float scale,
                                                 float& actual_scale) const override;

  //: Get a complete view from the image in the pyramid closest to the specified scale.
  // The scale factor is with respect to the base image (base scale = 1.0).
  // This method needs no specialization.
  vil_image_view_base_sptr get_copy_view(const float scale, float& actual_scale) const override
  { return get_copy_view(0, ni(), 0, nj(), scale, actual_scale); }

  //: Copy a resource into the pyramid, level is determined by resource scale
  bool put_resource(vil_image_resource_sptr const& /*resc*/) override
  { return false; }

  //: Get an image resource from the pyramid at the specified level
  vil_image_resource_sptr get_resource(const unsigned level) const override;

  //: for debug purposes
  void print(const unsigned level) override;

 protected:
  vil_openjpeg_pyramid_image_resource();
  vil_image_resource_sptr openjpeg_sptr_;
  vil_openjpeg_image* ptr_;
};


#endif // vil_openjpeg_pyramid_image_resource_h_
