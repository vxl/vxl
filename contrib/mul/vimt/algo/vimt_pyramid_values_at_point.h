// This is mul/vimt/algo/vimt_pyramid_values_at_point.h
#ifndef vimt_pyramid_values_at_point_h_
#define vimt_pyramid_values_at_point_h_

//:
// \file
// \brief Function to sample a point at each level of an image pyramid
// \author Tim Cootes

//: Use bilinear to sample from each level of the image pyramid
//  \param samples: Filled with image_pyr.n_levels() values
//  \param dummy: Used to define type of image pyramid
template<class T>
void vimt_pyramid_values_at_point(const vimt_image_pyramid& image_pyr,
                                  vgl_point_2d<double> p,
                                  vcl_vector<double>& samples,
                                  T dummy, int plane=0)
{
  samples.resize(image_pyr.n_levels());
  for (unsigned L=image_pyr.lo();L<=image_pyr.hi();++L)
  {
    const vimt_image_2d_of<T>& image
            = static_cast<const vimt_image_2d_of<T>&>(image_pyr(L));
    samples[L] = vimt_bilin_interp_safe(image,p,plane);
  }
}

#endif // vimt_pyramid_values_at_point_h_


