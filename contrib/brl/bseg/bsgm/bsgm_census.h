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
bool bsgm_compute_census_img(
  const vil_image_view<vxl_byte>& img,
  int nbhd_diam,
  vil_image_view<vxl_uint_64>& census,
  vil_image_view<vxl_uint_64>& census_conf,
  int tol = 2 );


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

//: Generate a bit-set look-up table for a pre-allocated array of size 256
void bsgm_generate_bit_set_lut(
  unsigned char* lut );


#endif // bsgm_census_h
