#ifdef __GNUC__
#pragma implementation
#endif


#include <vcl_cstdlib.h>
#include <mil/mil_image_pyramid.h>
#include <mil/mil_image.h>
#include <vsl/vsl_indent.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mil_image_pyramid::mil_image_pyramid()
	: base_pixel_width_(1.0),scale_step_(2.0)
{
}

//=======================================================================
// Destructor
//=======================================================================

void mil_image_pyramid::deleteImages()
{
	for (int i=0;i<image_.size();++i)
		delete image_[i];
	image_.resize(0);
}

mil_image_pyramid::~mil_image_pyramid()
{
	deleteImages();
}

//: Resize to [lo,hi] pyramid, each level of which is a clone of im_type
void mil_image_pyramid::resize(int n_levels, const mil_image& im_type)
{
	if (image_.size()==n_levels && n_levels>0 && image_[0]->is_a()==im_type.is_a())
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
	return image_.size()-1;
}
		
int mil_image_pyramid::nLevels() const
{
	return image_.size();
}

		//: Image at level L
mil_image& mil_image_pyramid::operator()(int L)
{
	assert(L>=0 && L<image_.size());
	return *image_[L];
}
	
		//: Image at level L
const mil_image& mil_image_pyramid::operator()(int L) const
{
	assert(L>=0 && L<image_.size());
	return *image_[L];
}
	
		//: Mean width (in world coords) of pixels at level zero
double mil_image_pyramid::basePixelWidth() const
{
	return base_pixel_width_;
}

		//: Scaling per level
		//  Pixels at level L have width 
		//  basePixelWidth() * scaleStep()^L
double mil_image_pyramid::scaleStep() const
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
	for (int i=0;i<image_.size();++i)
		os<<"Image at level "<<i<<" : "<<image_[i]<<vcl_endl;
}

//: Print whole of each image to os
void mil_image_pyramid::print_all(vcl_ostream& os) const
{
	os<<"Levels: "<<image_.size()<<vcl_endl;
	for (int i=0;i<image_.size();++i)
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


