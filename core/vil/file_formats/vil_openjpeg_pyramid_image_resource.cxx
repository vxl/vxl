#include <cmath>
#include "vil_openjpeg_pyramid_image_resource.h"
//:
// \file
// Do not remove the following notice
// Approved for public Release, distribution unlimited
// DISTAR Case 14074

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// By definition, each level is a factor of 2 reduced in scale
static float scale_at_level(unsigned level)
{
  if (level == 0)
    return 1.0f;
  float s = std::pow(2.0f, -static_cast<float>(level));
  return s;
}

vil_openjpeg_pyramid_image_resource::
vil_openjpeg_pyramid_image_resource(vil_image_resource_sptr const &openjpeg)
: openjpeg_sptr_(openjpeg)
{
  ptr_ = nullptr;
  if (!openjpeg_sptr_)
    return;
  ptr_ = dynamic_cast<vil_openjpeg_image*>(openjpeg_sptr_.ptr());
}

unsigned vil_openjpeg_pyramid_image_resource::nplanes() const
{
  unsigned ret = 0;
  if (ptr_)
    ret =  ptr_->nplanes();
  return ret;
}

//: The number of pixels in each row.
// Dimensions:  Planes x ni x nj.
// This method refers to the base (max resolution) image
unsigned vil_openjpeg_pyramid_image_resource::ni() const
{
  unsigned ret = 0;
  if (ptr_)
    ret = ptr_->ni();
  return ret;
}

//: The number of pixels in each column.
// Dimensions:  Planes x ni x nj.
// This method refers to the base (max resolution) image
unsigned vil_openjpeg_pyramid_image_resource::nj() const
{
  unsigned ret = 0;
  if (ptr_)
    ret =  ptr_->nj();
  return ret;
}

//: Pixel Format.
vil_pixel_format vil_openjpeg_pyramid_image_resource::pixel_format() const
{
  if (ptr_)
    return  ptr_->pixel_format();
  return VIL_PIXEL_FORMAT_UNKNOWN;
}

//: Return a string describing the file format.
// Only file images have a format, others return 0
char const* vil_openjpeg_pyramid_image_resource::file_format() const
{
  return "openjpeg_pyramid";
}


  // === Methods particular to pyramid resource ===

//: Number of pyramid levels.
unsigned vil_openjpeg_pyramid_image_resource::nlevels() const
{
  if (!ptr_)
    return 0;
  return ptr_->nreductions() + 1;
}

//: Get a partial view from the image from a specified pyramid level
vil_image_view_base_sptr
vil_openjpeg_pyramid_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned level) const
{
  if (!ptr_||!(ptr_->is_valid()))
     return nullptr;
  if (level >= this->nlevels())
    level = this->nlevels() - 1;
  return ptr_->get_copy_view_reduced(i0, ni, j0, nj, level);
}

//: Get a partial view from the image in the pyramid closest to scale.
// The origin and size parameters are in the coordinate system of the base image.
// The scale factor is with respect to the base image (base scale = 1.0).
vil_image_view_base_sptr
vil_openjpeg_pyramid_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   const float scale,
                                                   float& actual_scale) const
{
  if (scale >= 1.0f)
  {
    actual_scale = 1.0f;
    return this->get_copy_view(i0, ni, j0, nj, 0);
  }
  float f_lev = -std::log(scale) / std::log(2.0f);
  auto level = static_cast<unsigned>(f_lev);
  if (level >= this->nlevels())
    level = this->nlevels()-1;
  actual_scale = scale_at_level(level);
  return this->get_copy_view(i0, ni, j0, nj, level);
}

//: Get an image resource from the pyramid at the specified level
vil_image_resource_sptr
vil_openjpeg_pyramid_image_resource::get_resource(const unsigned level) const
{
  if (level == 0)
    return openjpeg_sptr_;
  return nullptr;
}

//: for debug purposes
void vil_openjpeg_pyramid_image_resource::print(const unsigned /*level*/)
{
}
