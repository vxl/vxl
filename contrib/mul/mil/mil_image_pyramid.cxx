// This is mul/mil/mil_image_pyramid.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "mil_image_pyramid.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <mil/mil_image.h>

//=======================================================================

mil_image_pyramid::mil_image_pyramid()
    : base_pixel_width_(1.0),scale_step_(2.0)
{
}

//=======================================================================

void mil_image_pyramid::deleteImages()
{
    for (unsigned int i=0;i<image_.size();++i)
        delete image_[i];
    image_.resize(0);
}

mil_image_pyramid::~mil_image_pyramid()
{
    deleteImages();
}

//=======================================================================
//: Copy operator
// Makes a shallow copy of each mil_image object, not of the
// underlying data
const mil_image_pyramid&
    mil_image_pyramid::operator=(const mil_image_pyramid& that)
{
    base_pixel_width_ = that.base_pixel_width_;
    scale_step_ = that.scale_step_;
    image_.resize(that.image_.size());
    for (unsigned int i=0;i<image_.size();++i)
        image_[i] = that.image_[i]->clone();

    return *this;
}

//=======================================================================
//: Copy ctor
mil_image_pyramid::mil_image_pyramid(const mil_image_pyramid &that)
{
  this->operator=(that);
}

//: Resize to [lo,hi] pyramid, each level of which is a clone of im_type
void mil_image_pyramid::resize(int n_levels, const mil_image& im_type)
{
    if (int(image_.size())==n_levels && n_levels>0 && image_[0]->is_a()==im_type.is_a())
        return;
    deleteImages();
    image_.resize(n_levels);
    for (int i=0;i<n_levels;++i)
        image_[i]=im_type.clone();
}

        //: Lowest level of pyramid
int mil_image_pyramid::lo() const
{
    return 0;
}

        //: Highest level
int mil_image_pyramid::hi() const
{
    return ((int)image_.size())-1;
}

int mil_image_pyramid::n_levels() const
{
    return image_.size();
}

        //: Image at level L
mil_image& mil_image_pyramid::operator()(int L)
{
    assert(L>=0 && (unsigned int)L<image_.size());
    return *image_[L];
}

        //: Image at level L
const mil_image& mil_image_pyramid::operator()(int L) const
{
    assert(L>=0 && (unsigned int)L<image_.size());
    return *image_[L];
}

        //: Mean width (in world coordinates) of pixels at level zero
double mil_image_pyramid::basePixelWidth() const
{
    return base_pixel_width_;
}

        //: Scaling per level
        //  Pixels at level L have width
        //  basePixelWidth() * scaleStep()^L
double mil_image_pyramid::scale_step() const
{
    return scale_step_;
}

        //: Access to image data
        //  Should only be used by pyramid builders
vcl_vector<mil_image*>& mil_image_pyramid::data()
{
    return image_;
}

        //: Define pixel widths
void mil_image_pyramid::setWidths(double base_pixel_width,
                                  double scale_step)
{
    base_pixel_width_ = base_pixel_width;
    scale_step_ = scale_step;
}

void mil_image_pyramid::print_summary(vcl_ostream& os) const
{
    os<<"Levels: "<<image_.size()<<vcl_endl;
    for (unsigned int i=0;i<image_.size();++i)
        os<<"Image at level "<<i<<" : "<<image_[i]<<vcl_endl;
}

//: Print whole of each image to os
void mil_image_pyramid::print_all(vcl_ostream& os) const
{
    os<<"Levels: "<<image_.size()<<vcl_endl;
    for (unsigned int i=0;i<image_.size();++i)
    {
        os<<"Image at level "<<i<<" : ";
        image_[i]->print_all(os);
    }
}

vcl_ostream& operator<<(vcl_ostream& os, const mil_image_pyramid& im_pyr)
{
    im_pyr.print_summary(os);
    return os;
}

vcl_ostream& operator<<(vcl_ostream& os, const mil_image_pyramid* im_pyr)
{
    if (im_pyr)
        im_pyr->print_summary(os);
    else
        os<<"NULL";
    return os;
}

void vsl_print_summary(vcl_ostream& os, const mil_image_pyramid& im_pyr)
{
  os << im_pyr;
}

void vsl_print_summary(vcl_ostream& os, const mil_image_pyramid* im_pyr)
{
  os << im_pyr;
}
