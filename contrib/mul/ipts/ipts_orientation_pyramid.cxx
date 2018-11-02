// This is mul/ipts/ipts_orientation_pyramid.cxx
#include "ipts_orientation_pyramid.h"
//:
// \file
// \brief Compute edge orientations at each level of a scale space pyramid
// \author Tim Cootes

#include <vil/algo/vil_orientations.h>
#include <vimt/vimt_image_2d_of.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute edge orientations at each level of a scale space pyramid.
//  smooth_pyramid must be of type float. orient_pyramid is set to be of type vxl_byte.
//  Uses vil_orientations_at_edges() on each level of the pyramid.
void ipts_orientation_pyramid(const vimt_image_pyramid& smooth_pyramid,
                              vimt_image_pyramid& orient_pyramid,
                              float grad_threshold,
                              unsigned n_orientations)
{
  if (smooth_pyramid.n_levels()==0) return;

  assert(smooth_pyramid(0).is_a()=="vimt_image_2d_of<float>");

  int n_levels = smooth_pyramid.n_levels();

  // Compute entropies for all levels of an image pyramid
  orient_pyramid.resize(n_levels,vimt_image_2d_of<vxl_byte>());
  for (int i=0;i<n_levels;++i)
  {
    const auto& smooth_im
            = static_cast<const vimt_image_2d_of<float>&>(smooth_pyramid(i));
    auto& orient_im
            = static_cast<vimt_image_2d_of<vxl_byte>&>(orient_pyramid(i));

    vil_image_view<float> grad_i,grad_j,grad_mag;
    vil_sobel_3x3(smooth_im.image(),grad_i,grad_j);
    vil_orientations_at_edges(grad_i,grad_j,orient_im.image(),grad_mag,
                              grad_threshold,n_orientations);

    orient_im.set_world2im(smooth_im.world2im());
  }
}
