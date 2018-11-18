#ifndef brec_pair_density_h_
#define brec_pair_density_h_
//:
// \file
// \brief Base class for prob density of an intensity pair
//
// \author Ozge C Ozcanli (ozge at lems dot brown dot edu)
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
  virtual ~brec_pair_density() = default;

  void set_sigma(double s) { sigma_ = s; }
  double sigma() const { return sigma_; }
  virtual double operator()(const double y0, const double y1) = 0;

  virtual vil_image_view<float> prob_density(vil_image_view<float>& obs);

 protected:
  double sigma_;
};

#endif // brec_pair_density_h_
