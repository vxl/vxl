// This is mul/clsfy/clsfy_parzen_builder.cxx
// Copyright (c) 2001: British Telecommunications plc
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Implement a Parzen window classifier builder
// \author Ian Scott
// \date 2001/10/07

#include "clsfy_parzen_builder.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_loader.h>
#include <clsfy/clsfy_rbf_parzen.h>

//=======================================================================

clsfy_parzen_builder::clsfy_parzen_builder():
sigma_(1.0), power_(2.0)
{
}


//=======================================================================

short clsfy_parzen_builder::version_no() const
{
  return 1;
}

//=======================================================================

vcl_string clsfy_parzen_builder::is_a() const
{
  return vcl_string("clsfy_parzen_builder");
}

//=======================================================================

bool clsfy_parzen_builder::is_class(vcl_string const& s) const
{
  return s == clsfy_parzen_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_parzen_builder::clone() const
{
  return new clsfy_parzen_builder(*this);
}

//=======================================================================

void clsfy_parzen_builder::print_summary(vcl_ostream& os) const
{
  os << "rbf width = " << sigma_ << ", power = "<< power_;
}

//=======================================================================

void clsfy_parzen_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, sigma_);
  vsl_b_write(bfs, power_);
}

//=======================================================================

void clsfy_parzen_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, sigma_);
    vsl_b_read(bfs, power_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_parzen_builder&)\n"
             << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_parzen_builder::build(clsfy_classifier_base& model,
                                   mbl_data_wrapper<vnl_vector<double> >& inputs,
                                   unsigned /* nClasses */,
                                   const vcl_vector<unsigned> &outputs) const
{
  const unsigned n = inputs.size();
  assert(model.is_class("clsfy_rbf_parzen")); // equiv to dynamic_cast<> != 0
  assert(n==outputs.size());

  clsfy_rbf_parzen &parzen = (clsfy_rbf_parzen&) model;

  vcl_vector<vnl_vector<double> > vin(inputs.size());

  inputs.reset();
  unsigned i=0;
  do
  {
    vin[i++] = inputs.current();
  } while (inputs.next());

  assert(i==n);

  parzen.set(vin, outputs);
  parzen.set_power(power_);
  parzen.set_rbf_width(sigma_);
  return clsfy_test_error(model, inputs, outputs);
}

//=======================================================================


//: Set the 1st standard deviation width of the RBF window.
// The default value is 1. Really this could be better named as the RBF radius.
void clsfy_parzen_builder::set_rbf_width(double sigma)
{
  assert(sigma > 0.0);
  sigma_=sigma;
}

//=======================================================================

//: The value p in the window function $exp(-1/(2*sigma^p) * |x-y|^p)$.
// The value p affects the kurtosis, or peakyness of the window.
// Towards 0 gives a more peaked central spike, and longer tail.
// Toward +inf gives a broader peak, and shorter tail.
// The default value is 2, giving a Gaussian distribution.
void clsfy_parzen_builder::set_power(double p)
{
  assert(p != 0.0);
  power_ = p;
}

//=======================================================================

//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_parzen_builder::new_classifier() const
{
  return new clsfy_rbf_parzen();
}

