// This is mul/clsfy/clsfy_logit_loss_function.h
#ifndef clsfy_logit_loss_function_h_
#define clsfy_logit_loss_function_h_
//:
// \file
// \brief Loss function for logit of linear classifier
// \author TFC

#include <vnl/vnl_cost_function.h>
#include <mbl/mbl_data_wrapper.h>

//: Loss function for logit of linear classifier.
//  For vector v' = (b w') (ie b=y[0], w=(y[1]...y[n])), computes
//  r(v) - (1/n_eg)sum log[(1-minp)logit(c_i * (b+w.x_i)) + minp]
//
// This is the sum of log prob of correct classification (+regulariser)
// which should be minimised to train the classifier.
//
// Note: Regularisor only important to deal with case where perfect
// classification possible, where scaling v would always increase f(v).
// Plausible choice of regularisor is clsfy_quad_regulariser (below)
class clsfy_logit_loss_function : public vnl_cost_function
{
private:
  mbl_data_wrapper<vnl_vector<double> >& x_;

  //: c[i] = -1 or +1, indicating class of x[i]
  const vnl_vector<double> & c_;

  //: Min probability (avoids log(zero))
  double min_p_;

  //: Optional regularising function
  vnl_cost_function *reg_fn_;
public:
  clsfy_logit_loss_function(mbl_data_wrapper<vnl_vector<double> >& x,
                            const vnl_vector<double> & c,
                            double min_p, vnl_cost_function* reg_fn=nullptr);

  //:  The main function: Compute f(v)
  double f(vnl_vector<double> const& v) override;

  //:  Calculate the gradient of f at parameter vector v.
  void gradf(vnl_vector<double> const& v,
                     vnl_vector<double>& gradient) override;

  //: Compute f(v) and its gradient (if non-zero pointers supplied)
  void compute(vnl_vector<double> const& v,
                       double *f, vnl_vector<double>* gradient) override;

};

//: Simple quadratic term used to regularise functions
//  For vector v' = (b w') (ie b=y[0], w=(y[1]...y[n])), computes
//  f(v) = alpha*|w|^2   (ie ignores first element, which is bias of linear classifier)
class clsfy_quad_regulariser : public vnl_cost_function
{
private:
  //: Scaling factor
  double alpha_;
public:
  clsfy_quad_regulariser(double alpha=1e-6);

  //:  The main function: Compute f(v)
  double f(vnl_vector<double> const& v) override;

  //:  Calculate the gradient of f at parameter vector v.
  void gradf(vnl_vector<double> const& v,
                     vnl_vector<double>& gradient) override;
};


#endif // clsfy_logit_loss_function_h_
