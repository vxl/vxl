// This is basic/dbil/algo/dbil_harr_wavelet_transform.h
#ifndef dbil_harr_wavelet_transform_h_
#define dbil_harr_wavelet_transform_h_
//:
// \file
// \brief Harr Wavelet Transform on Images
//
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 10/20/2005
//
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil/vil_image_view.h>


//: Compute the Harr wavelet transform on each plane
template< class T >
void dbil_harr_wavelet_transform(const vil_image_view<T>& src, 
                                       vil_image_view<T>& dest);  
                                       

//: Computes the inverse of the Harr wavelet transform on each plane
template< class T >
void dbil_harr_wavelet_inverse(const vil_image_view<T>& src, 
                                     vil_image_view<T>& dest);  


#endif // dbil_harr_wavelet_transform_h_
