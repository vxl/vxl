#ifndef ipts_orientation_pyramid_h_
#define ipts_orientation_pyramid_h_

//:
//  \file
//  \brief Compute edge orientations at each level of a scale space pyramid
//  \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid.h>


//: Compute edge orientations at each level of a scale space pyramid
//  smooth_pyramid must be of float. orient_pyramid  is set to be of type vxl_byte.
//  Uses vil_orientations_at_edges() on each level of the pyramid.
void ipts_orientation_pyramid(const vimt_image_pyramid& smooth_pyramid,
                         vimt_image_pyramid& orient_pyramid,
                         float grad_threshold,
                         unsigned n_orientations=255);

#endif // ipts_orientation_pyramid_h_
