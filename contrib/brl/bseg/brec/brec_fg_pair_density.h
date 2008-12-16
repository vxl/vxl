#ifndef brec_FG_PAIR_DENSITY_H_
#define brec_FG_PAIR_DENSITY_H_
//:
// \file
// \brief brec recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/01/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "brec_pair_density.h"


class brec_fg_pair_density : public brec_pair_density
{
 public:
  brec_fg_pair_density() : brec_pair_density() {}

  virtual double operator()(const double y0, const double y1);
  double negative_log(const double y0, const double y1);
  double gradient_of_log(const double y0, const double y1);
  double gradient_of_negative_log(const double y0, const double y1);
};

#endif // brec_FG_PAIR_DENSITY_H_
