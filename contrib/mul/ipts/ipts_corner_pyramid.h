#ifndef ipts_corner_pyramid_h_
#define ipts_corner_pyramid_h_

//:
//  \file
//  \brief Compute corner strength at each level of a scale space pyramid
//  \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid.h>

//: Compute corner strength at each level of a scale space pyramid
//  Build smooth gaussian pyramid from the image, then compute corners at each level.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_corner_pyramid(const vimt_image_2d_of<float>& image,
                         vimt_image_pyramid& corner_pyramid,
                         vimt_image_pyramid& smooth_pyramid,
                         double scale_step);

//: Compute corner strength at each level of a scale space pyramid
//  smooth_pyramid assumed to be of type float.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_corner_pyramid(const vimt_image_pyramid& smooth_pyramid,
                         vimt_image_pyramid& corner_pyramid);

#endif // ipts_corner_pyramid_h_
