//:
//  \file
//  \brief Compute local entropy at each level of a scale space pyramid
//  \author Tim Cootes

#include "ipts_entropy_pyramid.h"
#include "ipts_local_entropy.h"
#include <vimt/vimt_scale_pyramid_builder_2d.h>
#include <vcl_cassert.h>

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
                         double scale_step, unsigned half_width)
{
  vimt_scale_pyramid_builder_2d<vxl_byte> pyr_builder;
  pyr_builder.set_scale_step(scale_step);
  pyr_builder.build(smooth_pyramid,image);

  ipts_entropy_pyramid(smooth_pyramid,entropy_pyramid,half_width,0,255);
}

//: Compute corner strength at each level of a scale space pyramid
//  smooth_pyramid must be of vxl_byte.  entropy_pyramid is set to be of type float.
//  For each pixel in each image, compute entropy in region (2h+1)x(2h+1)
//  centred on the pixel.
//  Use ipts_scale_space_peaks() to get the position and scale of
//  likely corners
void ipts_entropy_pyramid(const vimt_image_pyramid& smooth_pyramid,
                         vimt_image_pyramid& entropy_pyramid,
                         unsigned half_width, int min_v, int max_v)
{
  if (smooth_pyramid.n_levels()==0) return;

  assert(smooth_pyramid(0).is_a()=="vimt_image_2d_of<vxl_byte>");

  // Entropy calculation translates results - allow for this
  vimt_transform_2d translate;
  translate.set_translation(-1.0*half_width,-1.0*half_width);

  // Work out how many levels and be used
  int n_levels = 0;
  for (int i=0;i<smooth_pyramid.n_levels();++i)
  {
    const vimt_image_2d_of<vxl_byte>& smooth_im
            = static_cast<const vimt_image_2d_of<vxl_byte>&>(smooth_pyramid(i));
     if (smooth_im.image().ni()>2*half_width+1 &&
         smooth_im.image().nj()>2*half_width+1)  n_levels++;
  }

  // Compute entropys for all levels of an image pyramid
  entropy_pyramid.resize(n_levels,vimt_image_2d_of<float>());
  for (int i=0;i<n_levels;++i)
  {
    const vimt_image_2d_of<vxl_byte>& smooth_im
            = static_cast<const vimt_image_2d_of<vxl_byte>&>(smooth_pyramid(i));
    vimt_image_2d_of<float>& entropy_im
            = static_cast<vimt_image_2d_of<float>&>(entropy_pyramid(i));
    ipts_local_entropy(smooth_im.image(),entropy_im.image(),half_width,min_v,max_v);

    entropy_im.set_world2im(translate*smooth_im.world2im());
  }

}


