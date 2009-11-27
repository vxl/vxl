#ifndef vil_threshold_h_
#define vil_threshold_h_
//:
// \file
// \brief Apply thresholds to image data
// \author Tim Cootes

#include <vil/vil_image_view.h>

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)>=t
// \relatesalso vil_image_view
template<class srcT>
void vil_threshold_above(const vil_image_view<srcT>& src,
                         vil_image_view<bool>& dest,  srcT t);

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t
// \relatesalso vil_image_view
template<class srcT>
void vil_threshold_below(const vil_image_view<srcT>& src,
                         vil_image_view<bool>& dest,  srcT t);

//: Apply threshold such that dest(i,j,p)=true if t0<=src(i,j,p)<=t1
// \relatesalso vil_image_view
template<class srcT>
void vil_threshold_inside(const vil_image_view<srcT>& src,
                          vil_image_view<bool>& dest,  srcT t0, srcT t1);

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t0 or src(i,j,p)>=t1
// \relatesalso vil_image_view
template<class srcT>
void vil_threshold_outside(const vil_image_view<srcT>& src,
                           vil_image_view<bool>& dest,  srcT t0, srcT t1);

#endif // vil_threshold_h_
