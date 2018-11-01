// This is mul/vimt/vimt_image_pyramid.cxx
//:
// \file

#include <iostream>
#include "vimt_image_pyramid.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <vimt/vimt_image.h>

//=======================================================================

vimt_image_pyramid::vimt_image_pyramid()
    : base_pixel_width_(1.0),scale_step_(2.0)
{
}

//=======================================================================

void vimt_image_pyramid::deleteImages()
{
    for (auto & i : image_)
        delete i;
    image_.resize(0);
}

vimt_image_pyramid::~vimt_image_pyramid()
{
    deleteImages();
}

//=======================================================================
//: Copy operator
// Makes a shallow copy of each vimt_image object, not of the
// underlying data
const vimt_image_pyramid&
    vimt_image_pyramid::operator=(const vimt_image_pyramid& that)
{
  if (&that == this) return *this;

  base_pixel_width_ = that.base_pixel_width_;
  scale_step_ = that.scale_step_;
  image_.resize(that.image_.size(),nullptr);
  for (unsigned int i=0;i<image_.size();++i)
  {
    delete image_[i];
    image_[i] = that.image_[i]->clone();
  }

  return *this;
}

//: Take a deep copy of the given pyramid
void vimt_image_pyramid::deep_copy(const vimt_image_pyramid& im_pyr)
{
  if (&im_pyr==this) return;

  base_pixel_width_ = im_pyr.base_pixel_width_;
  scale_step_ = im_pyr.scale_step_;
  image_.resize(im_pyr.image_.size(),nullptr);
  for (unsigned int i=0;i<image_.size();++i)
  {
    delete image_[i];
    image_[i] = im_pyr.image_[i]->deep_clone();
  }
}


//=======================================================================
//: Copy ctor
vimt_image_pyramid::vimt_image_pyramid(const vimt_image_pyramid &that)
{
  this->operator=(that);
}

//: Resize to [lo,hi] pyramid, each level of which is a clone of im_type
void vimt_image_pyramid::resize(int n_levels, const vimt_image& im_type)
{
    if (int(image_.size())==n_levels && n_levels>0 && image_[0]->is_a()==im_type.is_a())
        return;
    deleteImages();
    image_.resize(n_levels,nullptr);
    for (int i=0;i<n_levels;++i)
        image_[i]=im_type.clone();
}

//: Lowest level of pyramid
int vimt_image_pyramid::lo() const
{
    return 0;
}

//: Highest level
int vimt_image_pyramid::hi() const
{
    return ((int)image_.size())-1;
}

int vimt_image_pyramid::n_levels() const
{
    return image_.size();
}

//: Image at level L
vimt_image& vimt_image_pyramid::operator()(int L)
{
    assert(L>=0 && (unsigned int)L<image_.size());
    return *image_[L];
}

//: Image at level L
const vimt_image& vimt_image_pyramid::operator()(int L) const
{
    assert(L>=0 && (unsigned int)L<image_.size());
    return *image_[L];
}

//: Mean width (in world coordinates) of pixels at level zero
double vimt_image_pyramid::base_pixel_width() const
{
    return base_pixel_width_;
}

//: Scaling per level
//  Pixels at level L have width
//  basePixelWidth() * scaleStep()^L
double vimt_image_pyramid::scale_step() const
{
    return scale_step_;
}

//: Access to image data
//  Should only be used by pyramid builders
std::vector<vimt_image*>& vimt_image_pyramid::data()
{
    return image_;
}

//: Define pixel widths
void vimt_image_pyramid::set_widths(double base_pixel_width,
                                    double scale_step)
{
    base_pixel_width_ = base_pixel_width;
    scale_step_ = scale_step;
}

void vimt_image_pyramid::print_summary(std::ostream& os) const
{
    os<< vsl_indent() << "Levels: "<<image_.size()<<'\n';
    for (unsigned int i=0;i<image_.size();++i)
        os << vsl_indent() << "Image at level "<<i<<" : "<<image_[i]<<'\n';
}

//: Print whole of each image to os
void vimt_image_pyramid::print_all(std::ostream& os) const
{
    os<<"Levels: "<<image_.size()<<'\n';
    for (unsigned int i=0;i<image_.size();++i)
    {
        os<<"Image at level "<<i<<" : ";
        image_[i]->print_all(os);
        os<<'\n';
    }
}

std::ostream& operator<<(std::ostream& os, const vimt_image_pyramid& im_pyr)
{
    im_pyr.print_summary(os);
    return os;
}

std::ostream& operator<<(std::ostream& os, const vimt_image_pyramid* im_pyr)
{
    if (im_pyr)
        im_pyr->print_summary(os);
    else
        os<<"NULL";
    return os;
}

void vsl_print_summary(std::ostream& os, const vimt_image_pyramid& im_pyr)
{
  os << im_pyr;
}

void vsl_print_summary(std::ostream& os, const vimt_image_pyramid* im_pyr)
{
  os << im_pyr;
}
