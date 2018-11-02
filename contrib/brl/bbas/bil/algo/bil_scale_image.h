// This is brl/bbas/bil/algo/bil_scale_image.h
#ifndef bil_scale_image_h_
#define bil_scale_image_h_
//:
// \file
// \brief A scale-space image pyramid
//   This is the type of image pyramid described in
//  \verbatim
//   "Distinctive Image Features from Scale-Invariant Keypoints"
//    David G. Lowe,
//    International Journal of Computer Vision, 60, 2 (2004), pp. 91-110.
//  \endverbatim
//   This data structure represents and image using a pyramid over many scales.
//   The scales are grouped into "octaves" of images which have the same resolution.
//   Within an octave, images are blurred with a gaussian to produce images equally
//   distributed in scale space with scales between 1 and 2.  The last image with a
//   scale 2 is decimated by a factor of 2 to produce the first image in the next octave.
//   This is repeated for a specified number of octaves or until the image is two small.
//
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Wed Nov 19 2003
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>

template< class T >
class bil_scale_image
{
 public:
  //: Constructor
  bil_scale_image(unsigned int num_levels=3, unsigned int num_octaves=0,
                  float init_scale=1.0f, int first_octave=0);

  //: Constructor - Make a Gaussian scale space from this image
  bil_scale_image(const vil_image_view<T>& image, unsigned int num_levels=3,
                  unsigned int num_octaves=0, float init_scale=1.0f, int first_octave=0);

  //: Produce two new scale images by applying vil_orientations_from_sobel to each scale.
  //  The results are gradient magnitude and orientation scale images
  void compute_gradients(bil_scale_image<T>& orientation, bil_scale_image<T>& magnitude) const;

  //: Populate the scale space by Gaussian smoothing of \p image
  //  \param diff If not NULL, it is populated with the difference of Gaussians (DoG) scale space.
  void build_gaussian(const vil_image_view<T>& image, bil_scale_image<T>* diff = nullptr);

  //: Find the scale for the given octave and level
  float scale(int octave, unsigned int level) const
  {
    return (float) std::pow(2.0, octave + double(level)/(num_levels_)) * init_scale_;
  }

  //: Find how much the size of the image has been scaled for the given index
  //  This is useful for converting coordinates
  float image_scale(int octave) const
  {
    return (float)(1 << octave);
  }

  //: Find the closest scale to \p scale that is represented by an image
  float closest_scale(float scale) const
  {
    double log2_scale = std::log(scale/init_scale_) / vnl_math::ln2; // divide by log(2) to get base 2 log
    int s = int(log2_scale*num_levels_ +0.5);
    return (float) std::pow(2.0, double(s)/num_levels_) * init_scale_;
  }

  //: Return the number of levels
  unsigned int levels() const { return num_levels_; }

  //: Return the number of octaves
  unsigned int octaves() const { return data_.size(); }

  //: Return the initial scale
  float init_scale() const { return init_scale_; }

  int first_octave() const { return first_octave_; }

  //: Access by level and octave
  vil_image_view< T > & operator() (int octave, unsigned int level)
  { return data_[octave-first_octave_][level]; }

  //: Access by level and octave
  const vil_image_view< T > & operator() (int octave, unsigned int level) const
  { return data_[octave-first_octave_][level]; }

  //: create a 3x3x3 image estimating the 26 neighbors at location (i,j)
  vil_image_view< T > neighbors(int octave, unsigned int level,
                                unsigned int i, unsigned int j) const;

 private:
  //: The image data
  std::vector< std::vector< vil_image_view< T > > > data_;

  //: Number of images per octave
  unsigned int num_levels_;

  //: The scale of the initial image
  float init_scale_;

  //: The first octave level
  // typically zero but could be negative if we upsample the image
  int first_octave_;
};

#endif // bil_scale_image_h_
