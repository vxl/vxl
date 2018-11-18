// This is mul/clsfy/clsfy_binary_hyperplane_logit_builder.cxx
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "clsfy_binary_hyperplane_logit_builder.h"
//:
// \file
// \brief Linear classifier builder using a logit loss function
// \author Tim Cootes
// \date 18 Jul 2009

//=======================================================================

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_ref.h>
#include <vnl/algo/vnl_lbfgs.h>
#include <clsfy/clsfy_logit_loss_function.h>

clsfy_binary_hyperplane_logit_builder::clsfy_binary_hyperplane_logit_builder():
      clsfy_binary_hyperplane_ls_builder(),
      alpha_(1e-6),min_p_(0.001) {}

//: Weighting on regularisation term
void clsfy_binary_hyperplane_logit_builder::set_alpha(double a)
{
  alpha_=a;
}

//: Min prob to be returned by classifier
void clsfy_binary_hyperplane_logit_builder::set_min_p(double p)
{
  min_p_=p;
}

//---------------------------------------------------------------------
//------- The builder member functions --------------------------------
//---------------------------------------------------------------------

//: Build a linear classifier, with the given data.
// Return the mean error over the training set.
// n_classes must be 1.
double clsfy_binary_hyperplane_logit_builder::build(clsfy_classifier_base& classifier,
                                          mbl_data_wrapper<vnl_vector<double> >& inputs,
                                          unsigned n_classes,
                                          const std::vector<unsigned>& outputs) const
{
  assert (n_classes == 1);
  return clsfy_binary_hyperplane_logit_builder::build(classifier, inputs, outputs);
}

//: Build a linear hyperplane classifier with the given data.
double clsfy_binary_hyperplane_logit_builder::build(clsfy_classifier_base& classifier,
                                        mbl_data_wrapper<vnl_vector<double> >& inputs,
                                        const std::vector<unsigned>& outputs) const
{
  // First let the base class get us a starting solution
  clsfy_binary_hyperplane_ls_builder::build( classifier,inputs,outputs);

  std::cout<<"Initial error:"<< clsfy_test_error(classifier, inputs, outputs) <<std::endl;

  unsigned n_egs = inputs.size();
  if (n_egs == 0)
  {
      std::cerr<<"WARNING - clsfy_binary_hyperplane_logit_builder::build called with no data\n";
      return 0.0;
  }

  assert(classifier.is_a()=="clsfy_binary_hyperplane");
  auto& plane = static_cast<clsfy_binary_hyperplane&>(classifier);

  // Set initial weights using initial LS hyperplane
  unsigned n_dim = plane.n_dims();
  vnl_vector<double> w(n_dim+1);
  w[0]=-1*plane.bias();
  for (unsigned i=0;i<n_dim;++i) w[1+i]=plane.weights()[i];

  // Set up cost function
  vnl_vector<double> c(n_egs);
  for (unsigned i=0;i<n_egs;++i) c[i]=2.0*(outputs[i]-0.5);  // =+/-1
  clsfy_quad_regulariser quad_reg(alpha_);
  clsfy_logit_loss_function cost_fn(inputs,c,min_p_,&quad_reg);

  // Minimise it
  vnl_lbfgs optimizer(cost_fn);
  optimizer.set_verbose(true);
  optimizer.set_f_tolerance(1e-7);
  optimizer.set_x_tolerance(1e-5);
  if (!optimizer.minimize(w))
  {
    std::cerr<<"vnl_lbfgs optimisation failed!"<<std::endl;
    std::cerr<<"Failure code: "<<optimizer.get_failure_code()<<std::endl;
  }

  vnl_vector_ref<double> new_wts(n_dim,&w[1]);

  plane.set(new_wts,-w[0]);

  return clsfy_test_error(classifier, inputs, outputs);
}

//=======================================================================

void clsfy_binary_hyperplane_logit_builder::b_write(vsl_b_ostream &bfs) const
{
  constexpr int version_no = 1;
  vsl_b_write(bfs, version_no);
  clsfy_binary_hyperplane_ls_builder::b_write(bfs);
  vsl_b_write(bfs,min_p_);
  vsl_b_write(bfs,alpha_);
}

//=======================================================================

void clsfy_binary_hyperplane_logit_builder::b_read(vsl_b_istream &bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      clsfy_binary_hyperplane_ls_builder::b_read(bfs);
      vsl_b_read(bfs,min_p_);
      vsl_b_read(bfs,alpha_);
      break;
    default:
      std::cerr << "I/O ERROR: clsfy_binary_hyperplane_logit_builder::b_read(vsl_b_istream&)\n"
                << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

std::string clsfy_binary_hyperplane_logit_builder::is_a() const
{
  return std::string("clsfy_binary_hyperplane_logit_builder");
}

//=======================================================================

bool clsfy_binary_hyperplane_logit_builder::is_class(std::string const& s) const
{
  return s == clsfy_binary_hyperplane_logit_builder::is_a() || clsfy_binary_hyperplane_ls_builder::is_class(s);
}

//=======================================================================

short clsfy_binary_hyperplane_logit_builder::version_no() const
{
  return 1;
}

//=======================================================================

void clsfy_binary_hyperplane_logit_builder::print_summary(std::ostream& os) const
{
  os << is_a();
}

//=======================================================================
clsfy_builder_base* clsfy_binary_hyperplane_logit_builder::clone() const
{
  return new clsfy_binary_hyperplane_logit_builder(*this);
}
