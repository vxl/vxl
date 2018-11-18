// This is brl/bseg/bapl/bapl_lowe_pyramid_set.cxx
//:
// \file

#include <iostream>
#include <cmath>
#include <sstream>
#include "bapl_lowe_pyramid_set.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_math.h>
#include <vil/vil_decimate.h>
#include <vil/algo/vil_orientations.h>
#include <brip/brip_vil_ops.h>
#include <bapl/bapl_lowe_keypoint.h>

#include <vil/vil_copy.h>
#include <cassert>

//: Constructor
bapl_lowe_pyramid_set::bapl_lowe_pyramid_set( const vil_image_resource_sptr& image,
                                              unsigned octave_size, unsigned num_octaves, bool verbose)
 : gauss_pyramid_(octave_size, num_octaves),
   dog_pyramid_(octave_size, num_octaves),
   grad_orient_pyramid_(octave_size, num_octaves),
   grad_mag_pyramid_(octave_size, num_octaves),
   num_octaves_(num_octaves),
   octave_size_(octave_size),
   verbose_(verbose)
{
  // determine the number of octaves if not provided
  if ( num_octaves == 0 ) {
    int min_size = (image->ni() < image->nj())?image->ni():image->nj();
    min_size *= 2;
    num_octaves_ = 1;
    //while ( (min_size/=2) > 64) ++num_octaves_;
    while ( (min_size/=2) > 4) ++num_octaves_;

    gauss_pyramid_.resize(num_octaves_);
    dog_pyramid_.resize(num_octaves_);
    grad_orient_pyramid_.resize(num_octaves_);
    grad_mag_pyramid_.resize(num_octaves_);
  }

  if (verbose) {
    std::cout << " number of octaves: " << num_octaves_ << std::endl;
  }

  // Cast into float and upsample by 2x
  vil_image_view<float> image2x;
  float dummy=0.0;
  vil_image_view_base_sptr imagef;
  imagef = vil_convert_stretch_range(dummy, image->get_view());
  vil_resample_bilin( vil_image_view<float>(*imagef), image2x, 0, 0, 0.5, 0, 0, 0.5,
                      2*imagef->ni(), 2*imagef->nj());

  // correct for artefacts of upsampling at the border
  int ni = image2x.ni(), nj = image2x.nj();
  for (int i=0; i<ni-1; ++i)
    image2x(i,nj-1) = image2x(i,nj-2);
  for (int j=0; j<nj; ++j)
    image2x(ni-1,j) = image2x(ni-2,j);

  //+++++++++++++++++++++++++++++++++++++++++++++

  vil_image_view<float> temp;

  // Initial smoothing
  //brip_gauss_filter(image2x, temp, 1, 7, vil_convolve_constant_extend );
  brip_gauss_filter(image2x, temp, 1.6, 13, vil_convolve_constant_extend );

  double reduction = std::sqrt(std::pow(2.0,2.0/octave_size_)-1);

  // create the Gaussian Pyramid
  for (int lvl=0; lvl<num_octaves_; ++lvl) {
    for (unsigned octsz=0; octsz<octave_size; ++octsz) {
      vil_copy_deep(temp, gauss_pyramid_(lvl,octsz));
      double scale = std::pow(2.0,double(octsz)/octave_size_);
      double sigma = scale*reduction;
      int size = 2*int(sigma*3.5+0.5)+1;
      assert(size > 0);
      int ni = gauss_pyramid_(lvl,octsz).ni();
      int nj = gauss_pyramid_(lvl,octsz).nj();
      int smaller = ni < nj ? ni : nj;
      if (size >= smaller) size = smaller - 1;
      brip_gauss_filter( gauss_pyramid_(lvl,octsz), temp,
                         sigma, size, vil_convolve_constant_extend );
      vil_math_image_difference( temp, gauss_pyramid_(lvl,octsz), dog_pyramid_(lvl,octsz));
    }
    temp = vil_decimate(temp,2);
  }

  // compute the gradient magnitude and orientation of each image in the gauss pyramid
  for (int lvl=0; lvl<num_octaves_; ++lvl) {
    for (unsigned octsz=0; octsz<octave_size; ++octsz) {
      vil_orientations_from_sobel( gauss_pyramid_(lvl,octsz),
                                   grad_orient_pyramid_(lvl,octsz),
                                   grad_mag_pyramid_(lvl,octsz) );
    }
  }
}


//: Accessor for the Gaussian pyramid
const vil_image_view<float>&
bapl_lowe_pyramid_set::gauss_at(float scale, float *actual_scale, float *rel_scale) const
{
  return this->pyramid_at(gauss_pyramid_, scale, actual_scale, rel_scale);
}


//: Accessor for the Difference of Gaussians pyramid
const vil_image_view<float>&
bapl_lowe_pyramid_set::dog_at(float scale, float *actual_scale, float *rel_scale) const
{
  return this->pyramid_at(dog_pyramid_, scale, actual_scale, rel_scale);
}


//: Accessor for the Gradient orientation pyramid
const vil_image_view<float>&
bapl_lowe_pyramid_set::grad_orient_at(float scale, float *actual_scale, float *rel_scale) const
{
  return this->pyramid_at(grad_orient_pyramid_, scale, actual_scale, rel_scale);
}


//: Accessor for the Gradient magnitude pyramid
const vil_image_view<float>&
bapl_lowe_pyramid_set::grad_mag_at(float scale, float *actual_scale, float *rel_scale) const
{
  return this->pyramid_at(grad_mag_pyramid_, scale, actual_scale, rel_scale);
}


//: Return image in the pyramid closest to scale, and adjust i and j if provided
const vil_image_view<float>&
bapl_lowe_pyramid_set::pyramid_at( const bapl_lowe_pyramid<float> & pyramid,
                                   float scale, float *actual_scale, float *rel_scale) const
{
  double log2_scale = std::log(scale*2.0)/std::log(2.0);
  int index = int(log2_scale*octave_size_ +0.5);
  int octave = index/octave_size_;
  int sub_index = index%octave_size_;

  if ( octave >= num_octaves_ ) {
    octave = num_octaves_-1;
    sub_index = octave_size_-1;
  }

  if ( actual_scale ) *actual_scale = std::pow(2.0f, float(octave-1));
  if ( rel_scale )    *rel_scale    = std::pow(2.0f, float(sub_index)/octave_size_);

  return pyramid(octave, sub_index);
}


inline float gaussian( float x, float y)
{
  return std::exp(-((x*x)+(y*y))/(128.0f));
}

//: Make the descriptor for the given keypoint
bool
bapl_lowe_pyramid_set::make_descriptor(bapl_lowe_keypoint* keypoint)
{
  vnl_vector_fixed<double,128> histograms(0.0);

  float actual_scale, ref_scale;
  auto key_scale = float(keypoint->scale());
  const vil_image_view<float> & grad_orient = grad_orient_at(key_scale, &actual_scale, &ref_scale);
  const vil_image_view<float> & grad_mag =  grad_mag_at(key_scale);

  float key_x = float(keypoint->location_i()) / actual_scale;
  float key_y = float(keypoint->location_j()) / actual_scale;
  double key_orient = keypoint->orientation();

  for (int hi=0; hi<4; ++hi) {
    for (int hj=0; hj<4; ++hj) {
      for (int i=4*hi; i<4*(hi+1); ++i) {
        for (int j=4*hj; j<4*(hj+1); ++j) {
          double x = ( (i-7.5)*std::cos(key_orient)
                      -(j-7.5)*std::sin(key_orient)) * ref_scale;
          double y = ( (i-7.5)*std::sin(key_orient)
                      +(j-7.5)*std::cos(key_orient)) * ref_scale;
          for (int c=0; c<4; ++c) {
            int xc = int(x+key_x) + c/2;
            int yc = int(y+key_y) + c%2;
            if ( xc>=0 && xc<int(grad_orient.ni()) &&
                 yc>=0 && yc<int(grad_orient.nj()) ) {
              float interp_x = 1.0f - std::fabs( key_x + float(x-xc) );
              float interp_y = 1.0f - std::fabs( key_y + float(y-yc) );
              float weight = grad_mag(xc,yc) * interp_x * interp_y
                           * gaussian((xc-key_x)/ref_scale, (yc-key_y)/ref_scale);
              auto orient = float(vnl_math::angle_0_to_2pi(grad_orient(xc,yc)-key_orient+vnl_math::pi));
              int bin = ((int(orient*15/float(vnl_math::twopi))+1)/2)%8;
              histograms[hi*32+hj*8+bin] += weight;
            }
          }
        }
      }
    }
  }
  keypoint->set_descriptor(histograms);

  return true;
}
