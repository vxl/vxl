// This is brl/bseg/bsgm/bsgm_census.h
#ifndef bsgm_census_h
#define bsgm_census_h

#include <vector>
#include <vil/vil_image_view.h>

//:
// \file
// \brief Construct a "soft" version of the census transform
// \author Thomas Pollard
// \date April 17, 2016


//: Compute a census image and census confidence.
// if pixel type is short or float then the
// tolerance will typically be larger than 2
// the caller is responsible for determining an
// appropriate value.
template <class T>
bool bsgm_compute_census_img(
  const vil_image_view<T>& img,
  int nbhd_diam,
  vil_image_view<vxl_uint_64>& census,
  vil_image_view<vxl_uint_64>& census_conf,
  int tol = 2 )
{
  int height = img.nj(), width = img.ni();
  T max_val = std::numeric_limits<T>::max();
  // Can't handle bigger patch sizes with current implementation.
  if( nbhd_diam > 7 ) return false;

  census.set_size( width, height );
  census_conf.set_size( width, height );

  int nbhd_rad = (nbhd_diam-1)/2;
  // Iterate over each pixel
  for( int y = nbhd_rad; y < height-nbhd_rad; y++ ){
    for( int x = nbhd_rad; x < width-nbhd_rad; x++ ){
      T val = img(x, y);
      if (val > 0)
        int great = 0;
      T center_max = T( std::min<T>( max_val, val+tol ) );
      T center_min = 0;
      if(val>tol)
        center_min =T(std::max<T>(0, val-tol));

      unsigned long long cen = 0;
      unsigned long long conf = 0;

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
          // center in a separate "confidence" bit
          conf <<= 1;
          if( *img_x2y2 <= center_min || *img_x2y2 >= center_max ) conf++;
        }
      }
      census(x,y) = cen;
      census_conf(x,y) = conf;
    }
  }
  return true;
};

//: Find the difference between two census bit-strings with confidences
inline unsigned long long bsgm_compute_diff_string(
  unsigned long long int cen1,
  unsigned long long int cen2,
  unsigned long long int conf1,
  unsigned long long int conf2 )
{
  // Differences between the two bit strings
  unsigned long long int d = cen1^cen2;

  // Bits where at least one string is confident
  unsigned long long int c = conf1|conf2;

  // Take only confident different bits
  return d&c;
}


// The below are two ways of computing hamming distance from:
// http://graphics.stanford.edu/~seander/bithacks.html


//: Compute the hamming distance of a difference bit-string using Brian
// Kernighan's algorithm.
inline unsigned char bsgm_compute_hamming_bk(
  unsigned long long int diff )
{
  unsigned char ham = 0;
  while( diff ){
    ++ham;
    diff &= diff-1;
  }

  return ham;
}

//: Compute the hamming distance of a difference bit-string using a bit-set
// look-up table.
inline unsigned char bsgm_compute_hamming_lut(
  unsigned long long int diff,
  unsigned char* lut,
  bool only_32_bits = false )
{
  if( only_32_bits ) return
    lut[diff & 0xff] +
    lut[(diff >> 8) & 0xff] +
    lut[(diff >> 16) & 0xff] +
    lut[(diff >> 24) & 0xff];

  else return
    lut[diff & 0xff] +
    lut[(diff >> 8) & 0xff] +
    lut[(diff >> 16) & 0xff] +
    lut[(diff >> 24) & 0xff] +
    lut[(diff >> 32) & 0xff] +
    lut[(diff >> 40) & 0xff] +
    lut[(diff >> 48) & 0xff] +
    lut[(diff >> 54) & 0xff];
}

void bsgm_generate_bit_set_lut(unsigned char* lut );

#endif // bsgm_census_h
