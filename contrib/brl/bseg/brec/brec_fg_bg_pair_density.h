#ifndef brec_fg_bg_pair_density_h_
#define brec_fg_bg_pair_density_h_
//:
// \file
// \brief brec recognition utilities
//
// \author Ozge C Ozcanli (ozge at lems dot brown dot edu)
// \date Oct 01, 2008
//
// \verbatim
//  Modifications
//   2008-12-29 Peter Vanroose - added explicit virtual destructor
// \endverbatim

#include "brec_pair_density.h"

class brec_fg_bg_pair_density : public brec_pair_density
{
 public:
  brec_fg_bg_pair_density() : brec_pair_density() {}
  ~brec_fg_bg_pair_density() override = default;

  double operator()(const double y0, const double y1) override;
};

#endif // brec_fg_bg_pair_density_h_
