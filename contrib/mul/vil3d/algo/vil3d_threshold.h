#ifndef vil3d_threshold_h_
#define vil3d_threshold_h_
//:
// \file
// \brief Apply thresholds to image data and create a binary output image
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>

//: Apply threshold such that dest(i,j,k,p)=true if src(i,j,k,p)>=t
template<class srcT>
void vil3d_threshold_above(const vil3d_image_view<srcT>& src,
                           vil3d_image_view<bool>& dest,  srcT t);

//: Apply threshold such that dest(i,j,k,p)=true if src(i,j,k,p)<=t
template<class srcT>
void vil3d_threshold_below(const vil3d_image_view<srcT>& src,
                           vil3d_image_view<bool>& dest,  srcT t);

//: Apply threshold such that dest(i,j,k,p)=true if t0<=src(i,j,k,p)<=t1
template<class srcT>
void vil3d_threshold_inside(const vil3d_image_view<srcT>& src,
                            vil3d_image_view<bool>& dest,  srcT t0, srcT t1);

//: Apply threshold such that dest(i,j,k,p)=true if src(i,j,k,p)<=t0 or src(i,j,k,p)>=t1
template<class srcT>
void vil3d_threshold_outside(const vil3d_image_view<srcT>& src,
                             vil3d_image_view<bool>& dest,  srcT t0, srcT t1);

#endif // vil3d_threshold_h_
