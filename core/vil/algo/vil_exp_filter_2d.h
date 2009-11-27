#ifndef vil_exp_filter_2d_h_
#define vil_exp_filter_2d_h_
//:
// \file
// \brief Apply exponential filter in two dimensions
// \author Tim Cootes

#include <vil/algo/vil_exp_filter_1d.h>

//: Apply exponential filter along i and j to src_im to produce dest_im
//  Symmetric exponential filter of the form exp(c*|j|) applied. c=log(k)
//  Uses fast recursive implementation.
// \relatesalso vil_image_view
template <class srcT, class destT, class accumT>
inline void vil_exp_filter_2d(const vil_image_view<srcT>& src_im,
                              vil_image_view<destT>& dest_im,
                              accumT ki, accumT kj)
{
  vil_image_view<destT> filtered_im;
  vil_exp_filter_i(src_im,filtered_im,ki);
  vil_exp_filter_j(filtered_im,dest_im,kj);
}

#endif // vil_exp_filter_2d_h_
