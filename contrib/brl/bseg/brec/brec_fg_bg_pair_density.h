#ifndef brec_FG_BG_PAIR_DENSITY_H_
#define brec_FG_BG_PAIR_DENSITY_H_
//:
// \file
// \brief brec recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct. 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "brec_pair_density.h"

class brec_fg_bg_pair_density : public brec_pair_density
{
 public:
  brec_fg_bg_pair_density() : brec_pair_density() {}

  virtual double operator()(const double y0, const double y1);
};

#endif // brec_FG_BG_PAIR_DENSITY_H_
