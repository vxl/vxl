#ifndef vil2_exp_filter_2d_h_
#define vil2_exp_filter_2d_h_
//:
//  \file
//  \brief Apply exponential filter in two dimensions
//  \author Tim Cootes

#include <vil2/algo/vil2_exp_filter_1d.h>

//: Apply exponential filter along i and j to src_im to produce dest_im
//  Symmetric exponential filter of the form exp(c*|j|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relates vil2_image_view
template <class srcT, class destT, class accumT>
inline void vil2_exp_filter_2d(const vil2_image_view<srcT>& src_im,
                               vil2_image_view<destT>& dest_im,
                               accumT ki, accumT kj)
{
  vil2_image_view<destT> filtered_im;
  vil2_exp_filter_i(src_im,filtered_im,ki);
  vil2_exp_filter_j(filtered_im,dest_im,kj);
}

#endif // vil2_exp_filter_2d_h_
