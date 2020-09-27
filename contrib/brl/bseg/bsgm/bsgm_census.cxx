#include "bsgm_census.h"


//: Generate a bit-set look-up table for a pre-allocated array of size 256
void bsgm_generate_bit_set_lut(unsigned char* lut ){
  lut[0] = 0;
  for (int b = 0; b < 256; b++ )
    lut[b] = (b&1) + lut[b/2];
}
