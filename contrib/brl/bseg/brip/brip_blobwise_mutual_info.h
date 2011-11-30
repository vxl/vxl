// This is brl/bseg/brip/brip_blobwise_mutual_info.h
#ifndef brip_blobwise_mutual_info_h_
#define brip_blobwise_mutual_info_h_
//:
// \file
// \brief Calculate patchwise mutual info between two images
// \author Matt Leotta
//

#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

//: Calculate the Mutual Information between the images.
template<class T>
void brip_blobwise_mutual_info ( const vil_image_view<T>& img1,
                                 const vil_image_view<T>& img2,
                                 const vil_image_view<bool>& mask,
                                       vil_image_view<T>& mi_img ); 


#endif // brip_blobwise_mutual_info_h_
