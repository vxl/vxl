// This is core/vil/algo/vil_dog_pyramid.h
#ifndef vil_dog_pyramid_h_
#define vil_dog_pyramid_h_

//: 
// \file
// \brief Compute a pyramid of difference of gaussian images
// \author Tim Cootes

#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_math.h>
#include <vcl_vector.h>

//: Compute a pyramid of difference of gaussian images
//  Computes each layer of a pyramid by smoothing
//  then computing the difference from the original image.
//  The smoothed is then subsampled using a reduction factor of 1.5
//  (ie each level is 2/3 the size of the level below) and
//  used to produced the next level.
//
//  min_size defines the smallest dimension (restricting the number
//  of levels that will be constructed)
//
//  This is useful for finding locally interesting points and their
//  associated scales - see "Object Recognition from Scale Invariant Features"
//  D.Lowe, ICCV1999, pp.1150-1157.
template<class T>
void vil_dog_pyramid(const vil_image_view<T>& src_image,
                     vcl_vector<vil_image_view<T> >& smooth_pyramid,
                     vcl_vector<vil_image_view<T> >& dog_pyramid,
                     unsigned min_size)
{
  // Compute number of levels to build
  int n = vcl_min(src_image.ni(),src_image.nj());
  int nL = 0;
  while (n>min_size) { nL++; n=(2*n)/3; }

  smooth_pyramid.resize(nL);
  dog_pyramid.resize(nL);

  vil_image_view<T> sub_sampled_image;
  
  if (nL==0) return;

  vil_gauss_filter_5tap_params smooth_params(1.41421);

  // First level

  vil_gauss_filter_5tap(src_image,smooth_pyramid[0],smooth_params,
                        dog_pyramid[0]);  // Workspace
  vil_math_image_difference(src_image,smooth_pyramid[0],dog_pyramid[0]);

  unsigned n_planes = src_image.nplanes();

  double scaling = 2.0/3.0;

  // Subsequent levels
  for (unsigned i=1;i<nL;++i)
  {
    // Subsample by a factor of 2/3
    // Note - this could be implemented more efficiently
    //        since bilinear is sampling at pixel positions 
    //        and on edges.
    unsigned ni = smooth_pyramid[i-1].ni();
    unsigned nj = smooth_pyramid[i-1].nj();
    ni = 2*ni/3;
    nj = 2*nj/3;
    sub_sampled_image.set_size(ni,nj,n_planes);
    vil_resample_bilin(smooth_pyramid[i-1],sub_sampled_image,
                       0.0,0.0, scaling,0.0,  0.0,scaling, ni,nj);

    vil_gauss_filter_5tap(sub_sampled_image,smooth_pyramid[i],
                          smooth_params,
                          dog_pyramid[i]);  // Workspace
    vil_math_image_difference(sub_sampled_image,smooth_pyramid[i],
                              dog_pyramid[i]);
  }
}


#endif
