//:
// \file
#include "bvgl_ray_pyramid.h"

//construct pyramid from finest level
bvgl_ray_pyramid::bvgl_ray_pyramid(vbl_array_2d<vgl_ray_3d<float> >& finest_level, unsigned numLevels)
{
  if (finest_level.rows() != finest_level.cols()) {
    std::cerr << "bvgl_ray_pyramid input not square\n";
  }

  //side length
  const vbl_array_2d<vgl_ray_3d<float> >& level0(finest_level);
  pyramid_.push_back(level0);

  //create the levels
  for (unsigned int l=1; l<numLevels; ++l) {
    vbl_array_2d<vgl_ray_3d<float> > scaled = scale_down(pyramid_[l-1]);
    pyramid_.push_back(scaled);
  }
}

vbl_array_2d<vgl_ray_3d<float> > bvgl_ray_pyramid::scale_down(vbl_array_2d<vgl_ray_3d<float> >& toScale)
{
  //initialize new half size ray image
  std::size_t side_len = toScale.rows()/2;
  vbl_array_2d<vgl_ray_3d<float> > scaled(side_len, side_len);

  //go through and average the vectors
  for (unsigned int i=0; i<side_len; ++i) {
    for (unsigned int j=0; j<side_len; ++j) {
      //get 2x2 patch
      vgl_vector_3d<float> dir(0.0, 0.0, 0.0);
      for (int prevI=0; prevI<2; ++prevI)
        for (int prevJ=0; prevJ<2; ++prevJ)
          dir = dir + toScale(2*i+prevI, 2*j+prevJ).direction();
      normalize(dir);
      scaled(i,j) = vgl_ray_3d<float>( toScale(2*i, 2*j).origin(), dir );
    }
  }
  return scaled;
}
