//:
//  \file
//  \brief Compute corner strength at each level of a scale space pyramid
//  \author Tim Cootes

#include "ipts_corner_pyramid.h"
#include <vil/algo/vil_corners.h>
#include <vimt/vimt_scale_pyramid_builder_2d.h>

//: Compute corner strength at each level of a scale space pyramid
//  Build smooth gaussian pyramid from the image, then compute corners at each level.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_corner_pyramid(const vimt_image_2d_of<float>& image,
                         vimt_image_pyramid& corner_pyramid,
                         vimt_image_pyramid& smooth_pyramid,
                         double scale_step)
{
  vimt_scale_pyramid_builder_2d<float> pyr_builder;
  pyr_builder.set_scale_step(scale_step);
  pyr_builder.build(smooth_pyramid,image);

  ipts_corner_pyramid(smooth_pyramid,corner_pyramid);
}

//: Compute corner strength at each level of a scale space pyramid
//  smooth_pyramid assumed to be of type float.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_corner_pyramid(const vimt_image_pyramid& smooth_pyramid,
                         vimt_image_pyramid& corner_pyramid)
{
  // Compute corners for all levels of an image pyramid
  corner_pyramid.resize(smooth_pyramid.n_levels(),vimt_image_2d_of<float>());
  for (int i=0;i<smooth_pyramid.n_levels();++i)
  {
    const vimt_image_2d_of<float>& smooth_im
            = static_cast<const vimt_image_2d_of<float>&>(smooth_pyramid(i));
    vimt_image_2d_of<float>& corner_im
            = static_cast<vimt_image_2d_of<float>&>(corner_pyramid(i));
    corner_im.set_world2im(smooth_im.world2im());
    vil_corners(smooth_im.image(),corner_im.image());
  }

}


