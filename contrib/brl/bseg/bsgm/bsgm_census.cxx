// This is brl/bseg/bsgm/bsgm_census.cxx

#include <algorithm>
#include "bsgm_census.h"


//---------------------------------------------------------
bool bsgm_compute_census_img(
  const vil_image_view<vxl_byte>& img,
  int nbhd_diam,
  vil_image_view<unsigned long long>& census,
  vil_image_view<unsigned long long>& census_conf,
  int tol )
{  
  int height = img.nj(), width = img.ni();

  // Can't handle bigger patch sizes with current implementation.
  if( nbhd_diam > 7 ) return false;

  census.set_size( width, height );
  census_conf.set_size( width, height );

  int nbhd_rad = (nbhd_diam-1)/2;
  int centerCount = nbhd_diam*nbhd_diam/2;

  // Iterate over each pixel
  for( int y = nbhd_rad; y < height-nbhd_rad; y++ ){
    for( int x = nbhd_rad; x < width-nbhd_rad; x++ ){

      unsigned char center_max = (unsigned short)( std::min( 255, img(x,y) + tol ) );
      unsigned char center_min = (unsigned short)( std::max( 0, img(x,y) - tol ) );

      unsigned long long cen = 0;
      unsigned long long conf = 0;
      int count = 0;

      for( int y2 = y-nbhd_rad; y2 <= y+nbhd_rad; y2++ ){
        for( int x2 = x-nbhd_rad; x2 <= x+nbhd_rad; x2++, count++ ){
          //if( count == centerCount ) continue;

          // Record the sign of the sample-to-center difference in a bit at
          // each pixel in a patch.
          cen <<= 1;
          if( img(x2,y2) < img(x,y) ) cen++;

          // Also record whether the pixel differs significantly from the 
          // center in a separate "confidence" bit
          conf <<= 1;
          if( img(x2,y2) <= center_min || img(x2,y2) >= center_max ) conf++;
        }
      }
      census(x,y) = cen;
      census_conf(x,y) = conf;
    }
  }
  return true;
};


//----------------------------------------------------
unsigned char bsgm_compute_hamming(
  unsigned long long int s1,
  unsigned long long int s2,
  unsigned long long int c1,
  unsigned long long int c2 )
{
  // Differences between the two bit strings
  unsigned long long int d = s1^s2;

  // Bits where at least one string is confident
  unsigned long long int c = c1|c2;

  // Take only confident different bits
  d = d&c;

  // Use Brian Kernighan's algorithm
  unsigned char h = 0;
  while( d ){
    ++h;
    d &= d-1;
  }
  return h;
};
