// This is brl/bbas/bil/algo/bil_scale_image.hxx
#ifndef bil_scale_image_hxx_
#define bil_scale_image_hxx_
//:
// \file

#include <iostream>
#include <cmath>
#include "bil_scale_image.h"

#include <vcl_compiler.h>
#include <vcl_cassert.h>

#include <vil/vil_math.h>
#include <vil/vil_resample_bilin.h>
#include <vil/algo/vil_orientations.h>

#include <brip/brip_vil_ops.h>

//: Constructor
template <class T>
bil_scale_image<T>::bil_scale_image(unsigned num_levels, unsigned num_octaves,
                                    float init_scale, int first_octave)
 : data_(num_octaves,std::vector<vil_image_view< T > >(num_levels+2)),
   num_levels_(num_levels), init_scale_(init_scale), first_octave_(first_octave)
{}


//: Constructor - Make a Gaussian scale space from this image
template <class T>
bil_scale_image<T>::bil_scale_image(const vil_image_view<T>& image, unsigned int num_levels,
                                    unsigned int num_octaves, float init_scale, int first_octave)
 : data_(num_octaves,std::vector< vil_image_view< T > >(num_levels+2)),
   num_levels_(num_levels), init_scale_(init_scale), first_octave_(first_octave)
{
  build_gaussian(image);
}


template < class T >
static vil_image_view< T >
down_sample(const vil_image_view< T >& in)
{
  // decimate
  vil_image_view<T> temp(in.memory_chunk(), in.top_left_ptr(),
                         (in.ni()+1)/2, (in.nj()+1)/2, in.nplanes(),
                         in.istep()*2, in.jstep()*2, in.planestep());
  vil_image_view<T> out;
  out.deep_copy(temp);
  return out;
}


template < class T >
static void
smooth(double sigma, const vil_image_view< T >& in,
       vil_image_view< T >& out)
{
  const double sqrt2 = std::sqrt(2.0);
  int num_iter = 1;
  int size = 2*int(sigma*3.5+0.5)+1;
  int max_size = std::min(in.ni(),in.nj());
  while (size >= max_size){
    sigma /= sqrt2;
    num_iter *= 2;
    size = 2*int(sigma*3.5+0.5)+1;
  }

  if (num_iter == 1){
    brip_gauss_filter( in, out, sigma, size,
                       vil_convolve_constant_extend );
    return;
  }

  vil_image_view< T > temp1(in.ni(), in.nj()), temp2(in.ni(), in.nj());
  brip_gauss_filter( in, temp1, sigma, size,
                     vil_convolve_constant_extend );
  for (int i=0; i<(num_iter/2)-1; ++i){
    brip_gauss_filter( temp1, temp2, sigma, size,
                       vil_convolve_constant_extend );
    brip_gauss_filter( temp2, temp1, sigma, size,
                       vil_convolve_constant_extend );
  }
  brip_gauss_filter( temp1, out, sigma, size,
                     vil_convolve_constant_extend );
}


//: Populate the scale space by Gaussian smoothing of \p image
//  \param diff If not NULL, it is populated with the difference of Gaussians (DoG) scale space.
template < class T >
void
bil_scale_image<T>::build_gaussian(const vil_image_view<T>& image,
                                   bil_scale_image<T>* diff)
{
  if (diff){
    diff->init_scale_ = this->init_scale_;
    diff->num_levels_ = this->num_levels_;
    diff->first_octave_ = this->first_octave_;
    diff->data_.resize(this->data_.size(),
                       std::vector< vil_image_view< T > >(this->num_levels_+2));
  }

  assert(first_octave_ == -1 || first_octave_ == 0);
  vil_image_view<T> temp;
  double sigma = 0.0;
  if (first_octave_ == -1){
    temp = vil_image_view<T>(2*image.ni(), 2*image.nj(), image.nplanes());
    vil_resample_bilin( image, temp, 0.0, 0.0, 0.5, 0.0, 0.0, 0.5, 2*image.ni()-1, 2*image.nj()-1);
    if (init_scale_ > 1.0)
      sigma = std::sqrt(init_scale_*init_scale_ - 1.0);
  }
  else{
    temp = image;
    if (init_scale_ > 0.5)
      sigma = std::sqrt(init_scale_*init_scale_ - 0.5);
  }

  data_[0][0] = vil_image_view<T>(temp.ni(),temp.nj(), temp.nplanes());
  if (sigma > 0.0)
    smooth(sigma, temp, data_[0][0]);
  else
    data_[0][0].deep_copy(temp);

  double k = std::pow(2.0, 1.0/num_levels_);
  // solve: scale^2 + sigma^2 = (k*scale)^2
  double init_sigma = init_scale_ * std::sqrt(k*k - 1.0);

  // create the Gaussian Pyramid
  for (unsigned int oc=0; oc<data_.size(); ++oc)
  {
    if (oc > 0){
      temp = data_[oc][0] = down_sample(data_[oc-1][num_levels_]);
    }
    sigma = init_sigma;
    for (unsigned int lvl=0; lvl<num_levels_+2; ++lvl){
      std::cout << "img("<<oc<<','<<lvl<<") - ("<<data_[oc][lvl].ni()<<','<<data_[oc][lvl].nj()<<") "<< sigma <<std::endl;
      if (lvl<num_levels_+1)
        temp = data_[oc][lvl+1] = vil_image_view<T>(temp.ni(),temp.nj(),temp.nplanes());
      else
        temp = vil_image_view<T>(temp.ni(),temp.nj(),temp.nplanes());
      // Smooth with a Gaussian filter
      smooth(sigma, data_[oc][lvl], temp);
      // compute DoG image if requested
      if (diff)
        vil_math_image_difference( temp, data_[oc][lvl], (*diff)(oc+this->first_octave_,lvl));

      // compute sigma for the next level
      sigma *= k;
    }
  }
}


//: Produce two new scale images by applying vil_orientations_from_sobel to each scale.
// The results are gradient magnitude and orientation scale images
template < class T >
void
bil_scale_image<T>::compute_gradients(bil_scale_image<T>& orientation,
                                      bil_scale_image<T>& magnitude) const
{
  orientation.init_scale_ = this->init_scale_;
  magnitude.init_scale_ = this->init_scale_;
  orientation.first_octave_ = this->first_octave_;
  magnitude.first_octave_ = this->first_octave_;
  orientation.num_levels_ = this->num_levels_;
  magnitude.num_levels_ = this->num_levels_;
  orientation.data_.resize(this->data_.size(),
                           std::vector< vil_image_view< T > >(this->num_levels_+2));
  magnitude.data_.resize(this->data_.size(),
                         std::vector< vil_image_view< T > >(this->num_levels_+2));

  // compute the gradient magnitude and orientation of each image in the gauss pyramid
  for (unsigned int oc=0; oc<data_.size(); ++oc)
    for (unsigned int lvl=0; lvl<num_levels_+2; ++lvl)
      vil_orientations_from_sobel( data_[oc][lvl],
                                   orientation(oc+this->first_octave_, lvl),
                                   magnitude(oc+this->first_octave_, lvl) );
}


//: create a 3x3x3 image estimating the 26 neighbors at location (i,j)
template < class T >
vil_image_view< T >
bil_scale_image<T>::neighbors(int octave, unsigned int level,
                              unsigned int x, unsigned int y) const
{
  assert( level > 0 && level <= num_levels_ );
  vil_image_view< T > neighbors(3,3,3);
  int o = octave - first_octave_;
  const vil_image_view< T > & above = data_[o][level+1];
  const vil_image_view< T > & current = data_[o][level];
  const vil_image_view< T > & below = data_[o][level-1];

  for (int i=-1; i<2; ++i){
    for (int j=-1; j<2; ++j){
      neighbors(i+1,j+1,0) = below(x+i, y+j);
      neighbors(i+1,j+1,1) = current(x+i, y+j);
      neighbors(i+1,j+1,2) = above(x+i, y+j);
    }
  }
  return neighbors;
}


#define BIL_SCALE_IMAGE_INSTANTIATE(T) \
template class bil_scale_image<T >

#endif // bil_scale_image_hxx_
