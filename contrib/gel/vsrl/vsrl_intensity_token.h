#ifndef vsrl_intensity_token_h
#define vsrl_intensity_token_h
//:
// \file
// This token represents an element which has a single value associated
// with it.

#include <vsrl/vsrl_token.h>
#include <vsrl/vsrl_image_correlation.h>

class vsrl_intensity_token : public vsrl_token
{
  vsrl_image_correlation *image_correlation_;

  double bias_; // the expected position of the associated token
  double bias_cost_; // the cost for deviating from the bias
  double correlation_dyn_range_; // the standard deviation of the corelation function

  void compute_correlation_dyn_range(); // compute the correlation std

 public:

  // constructor
  vsrl_intensity_token();

  // destructor
  ~vsrl_intensity_token();

  // is this token an intensity token ?

  virtual bool intensity_token() {return true;}

  // what is the direct cost of assigning this token to tok
  virtual double cost(vsrl_token *tok);

  // set the image correlation mechanism
  void set_image_correlation(vsrl_image_correlation *image_correlation);

  // set the bias
  void set_bias(double bias);

  // set the bias cost
  void set_bias_cost(double bias_cost);

  // get the correlation_std
  double get_correlation_dyn_range();
};

#endif // vsrl_intensity_token_h
