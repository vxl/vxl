#ifndef brec_PAIR_DENSITY_H_
#define brec_PAIR_DENSITY_H_
//:
// \file
// \brief Base class for prob density of an intensity pair
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct. 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

class brec_pair_density
{
 public:

  brec_pair_density() : sigma_(0.1) {}

  void set_sigma(double s) { sigma_ = s; }
  double sigma() { return sigma_; }
  virtual double operator()(const double y0, const double y1) = 0;

  virtual vil_image_view<float> prob_density(vil_image_view<float>& obs);

 protected:
  double sigma_;
};

#endif // brec_PAIR_DENSITY_H_
