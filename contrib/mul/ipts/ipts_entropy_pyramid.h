#ifndef ipts_entropy_pyramid_h_
#define ipts_entropy_pyramid_h_

//:
//  \file
//  \brief Compute local entropy at each level of a scale space pyramid
//  \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid.h>

//: Compute local entropy at each level of a scale space pyramid
//  Build smooth gaussian pyramid from the image.
//  smooth_pyramid will be of vxl_byte.  entropy_pyramid is set to be of type float.
//  For each pixel in each image, compute entropy in region (2h+1)x(2h+1)
//  centred on the pixel.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_entropy_pyramid(const vimt_image_2d_of<vxl_byte>& image,
                         vimt_image_pyramid& entropy_pyramid,
                         vimt_image_pyramid& smooth_pyramid,
                         double scale_step, unsigned half_width);

//: Compute corner strength at each level of a scale space pyramid
//  smooth_pyramid must be of vxl_byte.   is set to be of type float.
//  For each pixel in each image, compute entropy in region (2h+1)x(2h+1)
//  centred on the pixel.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_entropy_pyramid(const vimt_image_pyramid& smooth_pyramid,
                         vimt_image_pyramid& entropy_pyramid,
                         unsigned half_width);

#endif // ipts_entropy_pyramid_h_
