// This is core/vil/algo/vil_2nd_deriv_3x3.h
#ifndef vil_2nd_deriv_3x3_h_
#define vil_2nd_deriv_3x3_h_
//:
// \file
// \brief Apply 3x3 2nd_derivative operator to image data
// \author J.L. Mundy

#include <vil/vil_image_view.h>

//: Compute 2nd derivatives of an image using 3x3 filters
//  Computes 2nd derivatives with respect to i,j and ij of an ni x nj plane of data
//  1 pixel border around 2nd derivative images is set to zero
// \relatesalso vil_image_view
template <class srcT, class destT>
void
vil_2nd_deriv_3x3(const vil_image_view<srcT> & src,
                  vil_image_view<destT> & d2I_di2,
                  vil_image_view<destT> & d2I_dj2,
                  vil_image_view<destT> & d2I_didj);

//: Compute 2nd derivatives of an image using 3x3 filters
//  Computes  i, j, and ij derivatives  of an nx x ny plane of data
//  d2I has three times as many planes as src, with dest plane (3i) being the 2nd i-derivative
//  of source plane i and dest plane (3i+1) being the 2nd j-derivative and dest plane (3i+2)
//  being the 2nd derivative with respect to i and j. A 1 pixel border around 2nd derivative images is set to zero
// \relatesalso vil_image_view
template <class srcT, class destT>
void
vil_2nd_deriv_3x3(const vil_image_view<srcT> & src, vil_image_view<destT> & d2I_dij);


//: Compute 2nd derivatives of single plane of 2D data using 3x3 filters
//  Computes 2nd derivatives with repect to i, j  and ij on a ni x nj plane of data
//  1 pixel border around 2nd derivative images is set to zero
template <class srcT, class destT>
void
vil_2nd_deriv_3x3_1plane(srcT * const src,
                         std::ptrdiff_t s_istep,
                         std::ptrdiff_t s_jstep,
                         destT * d2Idi,
                         std::ptrdiff_t d2Idi_istep,
                         std::ptrdiff_t d2Idi_jstep,
                         destT * d2Idj,
                         std::ptrdiff_t d2Idj_istep,
                         std::ptrdiff_t d2Idj_jstep,
                         destT * d2I_didj,
                         std::ptrdiff_t d2I_didj_istep,
                         std::ptrdiff_t d2I_didj_jstep,
                         unsigned ni,
                         unsigned nj);


#endif // vil_2nd_deriv_3x3_h_
