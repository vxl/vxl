// This is core/vil/vil_math.cxx
#include "vil_math.h"
//:
// \file
// \author Amitha Perera.
//
// \verbatim
//  Modifications
// \endvarbatim

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

void vil_math_median_unimplemented()
{
  vcl_cerr << "vil_math_median is currently not implemented for this data type\n";
  vcl_abort();
}

VCL_DEFINE_SPECIALIZATION
void vil_math_median(vxl_byte& median, const vil_image_view<vxl_byte>& im, unsigned p)
{
  unsigned ni = im.ni();
  unsigned nj = im.nj();

  // special case the empty image.
  if( ni*nj == 0 ) {
    median = 0;
    return;
  }

  unsigned hist[256] = { 0 };
  for (unsigned j=0;j<nj;++j) {
    for (unsigned i=0;i<ni;++i) {
      ++hist[ im(i,j,p) ];
    }
  }

  unsigned tot = ni*nj;
  // Target is ceil(tot/2)
  unsigned tgt = (tot+1) / 2;
  unsigned cnt = 0;
  unsigned idx = 0;
  while( cnt < tgt ) {
    cnt += hist[idx];
    ++idx;
  }

  // Test for halfway case
  if( cnt == tgt && tot % 2 == 0 ) {
    // idx is 
    unsigned lo = idx-1;
    while( hist[idx] == 0 ) {
      ++idx;
    }
    median = (lo + idx)/2;
  } else {
    median = idx-1;
  }
}
