#include "vsrl_intensity_token.h"
#include <vcl_iostream.h>


vsrl_intensity_token::vsrl_intensity_token()
{
  image_correlation_=0;
  bias_=0; // this is too dynamic so don't use the global parameters
  bias_cost_=0;
  correlation_dyn_range_=0;
}

// destructor

vsrl_intensity_token::~vsrl_intensity_token()
{
}

double vsrl_intensity_token::cost(vsrl_token *tok)
{
  // this is a null assignment
  if (tok->null_token()){
    // this is a null assignemnet
    return tok->cost(this);
  }

  // compute the cost due to the image data

  double image_cost=1.0;
  double delta_bias=0;

  if (tok->intensity_token()){
    // this is an intensity token so lets compare intensity

    if (image_correlation_){

      // we know how to perform image correlation so lets do it;
      int x1 = (int) this->get_x();
      int y1 = (int) this->get_y();

      double dx2,dy2;
      tok->get_position(dx2,dy2);
      int x2=(int)dx2;
      int y2=(int)dy2;

      // find the distance from the bias

      delta_bias=(x2-bias_);
      if (delta_bias <0){
        delta_bias = 0-delta_bias;
      }

      // compute the image correlation
      if (y1==y2){
        // we should be able to perform an efficient lookup
        image_cost= (1.0 - image_correlation_->get_correlation(x1,y1,(x2-x1)));
        if (image_cost <0.0){
          // vcl_cout << "Warning we have a strange correlation function" << vcl_endl;
          image_cost=1.0;
        }
      }
      else{
        // well it looks like we have to perform this calculation on the fly
        image_cost= (1.0 - image_correlation_->get_correlation(x1,y1,x2,y2));
      }
    }

    else{
      // we don't know how to compare these tokens
      vcl_cout << "warning cannot compare tokens" << vcl_endl;
    }
  }

  // find the cost due to delta_bias
  // the idea is that if the delta bias is large then we
  // want to incur a cost. However if the dynamic range
  // of the correlation function is close to 1 then we want
  // to ignore the delta bias

  // double delta_cost = sqrt(delta_bias) * bias_cost_ * (1.0 - correlation_dyn_range_);
  double delta_cost = (delta_bias) * bias_cost_;
  return delta_cost + image_cost;
}

void vsrl_intensity_token::set_image_correlation(vsrl_image_correlation *image_correlation)
{
  image_correlation_ = image_correlation;
  // compute the correlation std
  compute_correlation_dyn_range();
}

void vsrl_intensity_token::set_bias(double bias)
{
  bias_ = bias;
}

void vsrl_intensity_token::set_bias_cost(double bias_cost)
{
  bias_cost_=bias_cost;
}

double vsrl_intensity_token::get_correlation_dyn_range()
{
  return correlation_dyn_range_;
}

void vsrl_intensity_token::compute_correlation_dyn_range()
{
  // we want to compute the dynamic range of the
  // correlation. If it is high then we know that the
  // image has lots of information and there for it should
  // drive the assignment process. If it is low
  // then we should allow the dynamic program to be biased
  // by prior information such as the previous assignments

  correlation_dyn_range_=0;

  if (!image_correlation_){
    return;
  }

  // get the position of this
  int x1 = (int) this->get_x();
  int y1 = (int) this->get_y();

  double min_v=1;
  double max_v=0;

  double v;

  // get the range of correlations

  int range = image_correlation_->get_correlation_range();
  int i;

  for (i=0;i<2*range+1;i++){
    v= image_correlation_->get_correlation(x1,y1,(i-range));
    if (v<min_v){
      min_v=v;
    }
    if (v>max_v){
      max_v=v;
    }
  }

  if (min_v<0){
    min_v=0;
  }

  correlation_dyn_range_=max_v-min_v;
}
