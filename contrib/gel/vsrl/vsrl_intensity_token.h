#ifndef vsrl_intensity_token_h
#define vsrl_intensity_token_h


// This token represents an alement which has a single value associated
// with it.

#include<vsrl/vsrl_token.h>
#include<vsrl/vsrl_image_correlation.h>

class vsrl_intensity_token : public vsrl_token
{
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



 private:
  vsrl_image_correlation *_image_correlation;
 
  double _bias; // the expected posisition of the associated token
  double _bias_cost; // the cost for deviating from the bias
  double _correlation_dyn_range; // the standard deviation of the corelation function
  
  void compute_correlation_dyn_range(); // compute the correlation std
  

};

#endif
