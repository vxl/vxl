// This is brl/baml/baml_detect_change.cxx

#include <iomanip>
#include <algorithm>

#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_greyscale_erode.h>
#include <vil/algo/vil_greyscale_dilate.h>

#include "baml_birchfield_tomasi.h"


//--------------------------------------------------
template <class T>
bool baml_compute_birchfield_tomasi(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  vil_image_view<T>& score,
  int rad )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height )
    return false;

  // Initialize output image
  score.set_size( width, height );
  score.fill( (T)0 );

  // Compute neighborhood min and max
  vil_image_view<T> nbhd_min, nbhd_max;
  vil_structuring_element se;
  se.set_to_disk( rad + 0.01 );
  //static const int v_p_i0[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
  //static const int v_p_j0[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
  //std::vector< int > v_p_i(v_p_i0, v_p_i0+sizeof(v_p_i0)/sizeof(v_p_i0[0]));
  //std::vector< int > v_p_j(v_p_j0, v_p_j0+sizeof(v_p_j0)/sizeof(v_p_j0[0]));
  //se.set(v_p_i, v_p_j);
  vil_greyscale_dilate<T>( img_ref, nbhd_max, se );
  vil_greyscale_erode<T>( img_ref, nbhd_min, se );
  // Compute the Birchfield-Tomasi metric at each valid pixel
  for( int y = rad; y < height-rad; y++ ){
    for( int x = rad; x < width-rad; x++ ){

      if( img_tar(x,y) < nbhd_min(x,y) )
        score(x,y) = (T)( nbhd_min(x,y) - img_tar(x,y) );
      else if( img_tar(x,y) > nbhd_max(x,y) )
        score(x,y) = (T)( img_tar(x,y) - nbhd_max(x,y) );
    }
  }
  return true;
}


#undef BAML_COMPUTE_BIRCHFIELD_TOMASI_INSTANTIATE
#define BAML_COMPUTE_BIRCHFIELD_TOMASI_INSTANTIATE(T) \
template bool baml_compute_birchfield_tomasi( \
  const vil_image_view<T>& img_tar, \
  const vil_image_view<T>& img_ref, \
  vil_image_view<T>& score, \
  int rad );
