#ifndef brec_param_estimation_H_
#define brec_param_estimation_H_
//:
// \file
// \brief brec parameter estimation methods for various prob densities used for recognition
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <vil/vil_image_resource_sptr.h>
#include <bvgl/bvgl_changes_sptr.h>

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>

#include "brec_fg_pair_density.h"

struct fg_pair_density_est : public vnl_least_squares_function
{
  fg_pair_density_est(vcl_vector<vcl_pair<float, float> > const& pairs)
    : vnl_least_squares_function(1, pairs.size(), use_gradient),
    pairs_(pairs)
  { }

  void f(vnl_vector<double> const& x, vnl_vector<double>& y) {
    //double out = 0;
    brec_fg_pair_density fd;
    fd.set_sigma(x[0]);
    for (unsigned i = 0; i < pairs_.size(); i++) {
      //out += fd.gradient_of_log(pairs_[i].first, pairs_[i].second);
      //out += fd.negative_log(pairs_[i].first, pairs_[i].second);
      //y[i] = vcl_abs(fd.gradient_of_log(pairs_[i].first, pairs_[i].second));
      y[i] = fd.negative_log(pairs_[i].first, pairs_[i].second);
    }

    //y[0] = vcl_abs(out);
    //y[1] = vcl_abs(0.1-x[0]);
  }
  void gradf(vnl_vector<double> const& x, vnl_matrix<double> &J) {
    //J[0][0] = 0.0;
    brec_fg_pair_density fd;
    fd.set_sigma(x[0]);
    for (unsigned i = 0; i < pairs_.size(); i++) {
      //J[0][0] += fd.gradient_of_negative_log(pairs_[i].first, pairs_[i].second);
      J[i][0] = fd.gradient_of_negative_log(pairs_[i].first, pairs_[i].second);
    }
  }

  vcl_vector<vcl_pair<float, float> > pairs_;
};

class fg_pair_density_est_amoeba : public vnl_cost_function
{
 public:
  fg_pair_density_est_amoeba(vcl_vector<vcl_pair<float, float> > const& /*pairs*/): vnl_cost_function(1) {}

  double f(const vnl_vector<double>& x)
  {
    double out = 0;
    brec_fg_pair_density fd;
    fd.set_sigma(x[0]);
    for (unsigned i = 0; i < pairs_.size(); i++) {
      out += fd.negative_log(pairs_[i].first, pairs_[i].second);
    }

    return out;
  }

  vcl_vector<vcl_pair<float, float> > pairs_;
};

class brec_param_estimation
{
 public:

  //: estimate the initial value as the real variation in the data
  static double estimate_fg_pair_density_initial_sigma(vcl_vector<vcl_pair<float, float> >& pairs);

  //: we always assume that the intensities are scaled to [0,1] range, so we get a vector of float pairs
  static double estimate_fg_pair_density_sigma(vcl_vector<vcl_pair<float, float> >& pairs, double initial_sigma);

  //: we always assume that the intensities are scaled to [0,1] range, so we get a vector of float pairs
  static double estimate_fg_pair_density_sigma_amoeba(vcl_vector<vcl_pair<float, float> >& pairs, double initial_sigma);

  static bool create_fg_pairs(vil_image_resource_sptr img, bvgl_changes_sptr c, vcl_vector<vcl_pair<float, float> >& pairs, bool print_histogram = false, vcl_string out_name = "");
};

#endif // brec_param_estimation_H_

