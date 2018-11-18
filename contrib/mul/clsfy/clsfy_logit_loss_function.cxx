// This is mul/clsfy/clsfy_logit_loss_function.cxx
//:
// \file
// \brief Loss function for logit of linear classifier
// \author TFC

#include "clsfy_logit_loss_function.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

clsfy_logit_loss_function::clsfy_logit_loss_function(
            mbl_data_wrapper<vnl_vector<double> >& x,
            const vnl_vector<double> & c,
            double min_p, vnl_cost_function* reg_fn)
  : x_(x),c_(c),min_p_(min_p),reg_fn_(reg_fn)
{
  x.reset();
  set_number_of_unknowns(x.current().size());
}

//: Assumes w.size()=x.size()+1. Computes (1 x')w
inline double dot1(const vnl_vector<double>& w,
                   const vnl_vector<double>& x)
{
  return w[0] + vnl_c_vector<double>::dot_product(w.begin()+1,
                                      x.begin(),
                                      x.size());
}

//:  The main function: Compute f(w)
double clsfy_logit_loss_function::f(vnl_vector<double> const& v)
{
  double sum=0;
  if (reg_fn_) sum=reg_fn_->f(v);
  x_.reset();
  double a=1.0/x_.size();
  for (unsigned i=0;i<x_.size();++i,x_.next())
  {
    double z = c_[i]*dot1(v,x_.current());
    sum -= a*std::log(min_p_+(1-min_p_)/(1+std::exp(-z)) );
  }
  return sum;
}

  //:  Calculate the gradient of f at parameter vector v.
void clsfy_logit_loss_function::gradf(vnl_vector<double> const& v,
                     vnl_vector<double>& gradient)
{
  x_.reset();
  unsigned n=x_.current().size();

  // Term from regulariser
  if (reg_fn_)
    reg_fn_->gradf(v,gradient);
  else
  {
    gradient.set_size(n+1);
    gradient.fill(0.0);
  }

  double a=1.0/x_.size();
  double *g1 = gradient.data_block()+1; // g[1] associated with x[0]
  assert(v.size()==n+1);
  for (unsigned i=0;i<x_.size();++i,x_.next())
  {
    double z = c_[i]*dot1(v,x_.current());
    double ez = std::exp(-1*z);
    double sz=1/(1+ez);
    double fi = min_p_+(1-min_p_)*sz;
    double k = a*(1-min_p_)*ez*sz*sz*c_[i]/fi;
    gradient[0]-=k;  // First element notionally 1.0
    const double* x = x_.current().data_block();
    for (unsigned j=0;j<n;++j) g1[j]-=k*x[j];
  }
}

void clsfy_logit_loss_function::compute(vnl_vector<double> const& v,
               double *f, vnl_vector<double>* gradient)
{
  if (!gradient)
  {
    if (f) *f = this->f(v);
    return;
  }

  x_.reset();
  unsigned n=x_.current().size();

  // Term from regulariser
  if (reg_fn_)
    reg_fn_->gradf(v,*gradient);
  else
  {
    gradient->set_size(n+1);
    gradient->fill(0.0);
  }


  double a=1.0/x_.size();
  double *g1 = gradient->data_block()+1; // g[1] associated with x[0]

  double sum=0;
  if (reg_fn_) sum=reg_fn_->f(v);

  assert(v.size()==n+1);
  for (unsigned i=0;i<x_.size();++i,x_.next())
  {
    double z = c_[i]*dot1(v,x_.current());
    double ez = std::exp(-1*z);
    double sz=1/(1+ez);
    double fi = min_p_+(1-min_p_)*sz;
    sum -= a*std::log(fi);
    double k = a*(1-min_p_)*ez*sz*sz*c_[i]/fi;

    // Update gradient with k*x (extending x by 1 in first element)
    (*gradient)[0]-=k;  // First element notionally 1.0
    const double* x = x_.current().data_block();
    for (unsigned j=0;j<n;++j) g1[j]-=k*x[j];
  }
  if (f) *f = sum;
}

clsfy_quad_regulariser::clsfy_quad_regulariser(double alpha)
  : alpha_(alpha)
{
}

//:  The main function: Compute f(v)
double clsfy_quad_regulariser::f(vnl_vector<double> const& v)
{
  double sum=0.0;
  for (unsigned i=1;i<v.size();++i) sum+=v[i]*v[i];
  return alpha_*sum;
}

//:  Calculate the gradient of f at parameter vector v.
void clsfy_quad_regulariser::gradf(vnl_vector<double> const& v,
                     vnl_vector<double>& gradient)
{
  gradient = (2*alpha_)*v;
  gradient[0]=0.0;  // Function independent of first element
}
