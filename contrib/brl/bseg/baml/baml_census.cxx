// This is brl/bseg/baml/baml_census.cxx

#include <algorithm>
#include "baml_census.h"


//---------------------------------------------------------
template <class T>
bool
baml_compute_census_img(
  const vil_image_view<T>& img,
  int nbhd_diam,
  vil_image_view<vxl_uint_64>& census,
  vil_image_view<vxl_uint_64>& census_sal,
  int tol )
{
  int height = img.nj(), width = img.ni();

  // Can't handle bigger patch sizes with current implementation.
  if( nbhd_diam > 7 ) return false;

  census.set_size( width, height );
  census_sal.set_size( width, height );

  int nbhd_rad = (nbhd_diam-1)/2;
  int max_intensity = pow( 256, sizeof(T) )-1;

  // Iterate over each pixel
  for( int y = nbhd_rad; y < height-nbhd_rad; y++ ){
    for( int x = nbhd_rad; x < width-nbhd_rad; x++ ){

      T center_max = (T)( std::min( max_intensity, img(x,y) + tol ) );
      T center_min = (T)( std::max( 0, img(x,y) - tol ) );

      unsigned long long cen = 0;
      unsigned long long sal = 0;

      int x_min = x-nbhd_rad, y_min = y-nbhd_rad;
      T img_xy = img(x,y);
      for( int dy = 0; dy < nbhd_diam; dy++ ){
        const T* img_x2y2 = &img( x_min, y_min + dy );
        for( int dx = 0; dx < nbhd_diam; dx++, img_x2y2++ ){

          // Record the sign of the sample-to-center difference in a bit at
          // each pixel in a patch.
          cen <<= 1;
          if( *img_x2y2 < img_xy ) cen++;

          // Also record whether the pixel differs significantly from the
          // center in a separate "salience" bit
          sal <<= 1;
          if( *img_x2y2 <= center_min || *img_x2y2 >= center_max ) sal++;
        }
      }
      census(x,y) = cen;
      census_sal(x,y) = sal;
    }
  }
  return true;
};

#undef BAML_COMPUTE_CENSUS_IMG_INSTANTIATE
#define BAML_COMPUTE_CENSUS_IMG_INSTANTIATE(T) \
template bool baml_compute_census_img( \
  const vil_image_view<T>& img, \
  int nbhd_diam, \
  vil_image_view<vxl_uint_64>& census, \
  vil_image_view<vxl_uint_64>& census_sal, \
  int tol )
