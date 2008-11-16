//:
// \file
// \brief bvxm recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/01/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//


#if !defined(_DBVXM_FG_BG_PAIR_DENSITY_H)
#define _DBVXM_FG_BG_PAIR_DENSITY_H

#include "bvxm_pair_density.h"

class bvxm_fg_bg_pair_density : public bvxm_pair_density
{
public:
  bvxm_fg_bg_pair_density() : bvxm_pair_density() {}

  virtual double operator()(const double y0, const double y1);

};

#endif  //_DBVXM_FG_BG_PAIR_DENSITY_H
