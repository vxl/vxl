#ifndef vil2_threshold_h_
#define vil2_threshold_h_
//:
//  \file
//  \brief Apply thresholds to image data
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)>=t
template<class srcT>
void vil2_threshold_above(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t);

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t
template<class srcT>
void vil2_threshold_below(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t);

//: Apply threshold such that dest(i,j,p)=true if t0<=src(i,j,p)<=t1
template<class srcT>
void vil2_threshold_inside(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t0, srcT t1);

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t0 or src(i,j,p)>=t1
template<class srcT>
void vil2_threshold_outside(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t0, srcT t1);

#endif // vil2_threshold_h_
