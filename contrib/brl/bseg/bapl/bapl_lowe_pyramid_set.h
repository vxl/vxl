// This is brl/bseg/bapl/bapl_lowe_pyramid_set.h
#ifndef bapl_lowe_pyramid_set_h_
#define bapl_lowe_pyramid_set_h_
//:
// \file
// \brief A class to create and store several types of pyramids of an image
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 15 2003
//
// \verbatim
//  Modifications
//  May 10, 2010 Andrew Hoelscher - Added verbose option to disable printing
// \endverbatim

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vbl/vbl_ref_count.h>
#include <bapl/bapl_lowe_pyramid.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>

//: This class create several types of pyramids from an image
//  - A Gaussian pyramid
//  - A Difference of Gaussian (DoG) pyramid
//  - Gradient magnitude pyramid
//  - Gradient orientation pyramid

class bapl_lowe_pyramid_set : public vbl_ref_count
{
 public:
  //: Constructor
  // if \param num_octaves is zero the number of octaves is determined from the image size
  bapl_lowe_pyramid_set( const vil_image_resource_sptr& image,
                         unsigned octave_size=3, unsigned num_octaves=0,
                         bool verbose=true);

  //: Accessor for the Gaussian pyramid
  const vil_image_view<float>& gauss_at( float scale,
                                         float *actual_scale=nullptr,
                                         float *rel_scale=nullptr ) const;
  //: Accessor for the Difference of Gaussians pyramid
  const vil_image_view<float>& dog_at( float scale,
                                       float *actual_scale=nullptr,
                                       float *rel_scale=nullptr ) const;
  //: Accessor for the Gradient orientation pyramid
  const vil_image_view<float>& grad_orient_at( float scale,
                                               float *actual_scale=nullptr,
                                               float *rel_scale=nullptr ) const;
  //: Accessor for the Gradient magnitude pyramid
  const vil_image_view<float>& grad_mag_at( float scale,
                                            float *actual_scale=nullptr,
                                            float *rel_scale=nullptr ) const;

  //: Accessor for the Gaussian pyramid
  const vil_image_view<float>& gauss_pyramid(unsigned octave, unsigned sub_index) const
                               { return gauss_pyramid_(octave, sub_index); }
  //: Accessor for the Difference of Gaussians pyramid
  const vil_image_view<float>& dog_pyramid(unsigned octave, unsigned sub_index) const
                               { return dog_pyramid_(octave, sub_index); }
  //: Accessor for the Gradient orientation pyramid
  const vil_image_view<float>& grad_orient_pyramid(unsigned octave, unsigned sub_index) const
                               { return grad_orient_pyramid_(octave, sub_index); }
  //: Accessor for the Gradient magnitude pyramid
  const vil_image_view<float>& grad_mag_pyramid(unsigned octave, unsigned sub_index) const
                               { return grad_mag_pyramid_(octave, sub_index); }

  //: create a 3x3x3 image of neighbors at a location in the gauss pyramid
  vil_image_view<float> gauss_neighbors(unsigned index, unsigned x, unsigned y) const
                        { return gauss_pyramid_.neighbors(index, x, y); }

  //: create a 3x3x3 image of neighbors at a location in the DoG pyramid
  vil_image_view<float> dog_neighbors(unsigned index, unsigned x, unsigned y) const
                        { return dog_pyramid_.neighbors(index, x, y); }

  //: Make the descriptor for the given keypoint
  bool make_descriptor(bapl_lowe_keypoint* keypoint);

  //: Accessor for the number of octaves
  int num_octaves() const { return num_octaves_; }
  //: Accessor for the size of an octave
  int octave_size() const { return octave_size_; }

 protected:
  //: Return image in the pyramid closest to scale, and adjust i and j if provided
  const vil_image_view<float>& pyramid_at( const bapl_lowe_pyramid<float> & pyramid,
                                           float scale, float *actual_scale=nullptr,
                                           float *rel_scale=nullptr ) const;

 private:
  //: Gaussian pyramid
  bapl_lowe_pyramid<float> gauss_pyramid_;
  //: Difference of Gaussians pyramid
  bapl_lowe_pyramid<float> dog_pyramid_;
  //: Gradient orientation pyramid
  bapl_lowe_pyramid<float> grad_orient_pyramid_;
  //: Gradient magnitude pyramid
  bapl_lowe_pyramid<float> grad_mag_pyramid_;

  int num_octaves_;
  int octave_size_;
  bool verbose_;
};

#endif // bapl_lowe_pyramid_set_h_
