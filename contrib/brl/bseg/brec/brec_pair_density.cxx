//:
// \file
// \brief Base class for prob density of an intensity pair
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "brec_pair_density.h"

vil_image_view<float>
brec_pair_density::prob_density(vil_image_view<float>& obs)
{
  unsigned ni = obs.ni();
  unsigned nj = obs.nj();

  vil_image_view<float> map(ni, nj);
  map.fill(0.0f);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if (i+1 < ni) {
        map(i,j) = (float)this->operator()(obs(i,j), obs(i+1, j));
      }
    }
  }

  return map;
}
