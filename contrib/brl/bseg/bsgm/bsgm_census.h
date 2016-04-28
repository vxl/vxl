// This is brl/bseg/bsgm/bsgm_census.h
#ifndef bsgm_census_h
#define bsgm_census_h

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
  vil_image_view<unsigned long long>& census,
  vil_image_view<unsigned long long>& census_conf,
  int tol = 2 );

//: Compute the hamming distance between two census pixels.
unsigned char bsgm_compute_hamming(
  unsigned long long int s1,
  unsigned long long int s2,
  unsigned long long int c1,
  unsigned long long int c2 );


#endif // bsgm_census_h
