// This bbas/vpgl/ihog/icam_sample.h
#ifndef icam_sample_h_
#define icam_sample_h_
//:
// \file
// \brief A class to sample an image with respect to a depth map
// \author J.L. Mundy
// \date Sept. 07, 2010
// The depth transform, dt,  provides a mapping of image pixel values from the
// source image to the destination camera frame. Each pixel location in the
// destination is mapped to the source and the intensity is bilinearly
// interpolated to obtain the mapped destination pixel value. These values
// are denoted as "samples" or "dest." A mask array or image is provided to
// indicate which samples are valid. A sample is invalid if its dest pixel
// coordinates map to a location outside the source image. A mask value of 1
// indicates valid, and 0 invalid.
//
#include <icam/icam_depth_transform.h>
#include <vil/vil_image_view.h>

class icam_sample
{
 public:
  //:
  //  given a depth transform, produce a 1-d array of samples
  // from a source image that is transformed with respect to the destination.
  // Used to match an image of an unknown camera (source) with respect to an
  // image with a known camera (dest). The mask array indicates which
  // samples are valid, i.e. mask == 1
  static void sample( unsigned int ni_dest,  unsigned int nj_dest,
                      vil_image_view<float> const& source,
                      icam_depth_transform const& dt,
                      vnl_vector<double>& samples,
                      vnl_vector<double>& mask,
                      unsigned& n_samples);

  //:
  //  given a depth transform from dest to source, produce an image
  //  is the resampling of the source to the destination. The mask
  //  indicates which pixels in the destination are valid, i.e. mask == 1
  static void resample(unsigned int ni_dest,  unsigned int nj_dest,
                       vil_image_view<float> const& source,
                       icam_depth_transform const& dt,
                       vil_image_view<float>& dest,
                       vil_image_view<float>& mask,
                       unsigned& n_samples);

  //: samples from externally processed images
  static void sample(vil_image_view<float> const& map_dest,
                     vil_image_view<float> const& map_mask,
                     vnl_vector<double>& samples,
                     vnl_vector<double>& mask,
                     unsigned& n_samples);
};

#endif // icam_sample_h_
