// This is mul/clsfy/clsfy_parzen_builder.cxx
// Copyright (c) 2001: British Telecommunications plc
//:
// \file
// \brief Implement a Parzen window classifier builder
// \author Ian Scott
// \date 2001-10-07

#include <iostream>
#include <sstream>
#include <string>
#include "clsfy_parzen_builder.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
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

std::string clsfy_parzen_builder::is_a() const
{
  return std::string("clsfy_parzen_builder");
}

//=======================================================================

bool clsfy_parzen_builder::is_class(std::string const& s) const
{
  return s == clsfy_parzen_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_parzen_builder::clone() const
{
  return new clsfy_parzen_builder(*this);
}

//=======================================================================

void clsfy_parzen_builder::print_summary(std::ostream& os) const
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
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_parzen_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
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
                                   const std::vector<unsigned> &outputs) const
{
  assert(model.is_class("clsfy_rbf_parzen")); // equiv to dynamic_cast<> != 0
  assert(inputs.size()==outputs.size());

  auto &parzen = (clsfy_rbf_parzen&) model;

  std::vector<vnl_vector<double> > vin(inputs.size());

  inputs.reset();
  unsigned i=0;
  do
  {
    vin[i++] = inputs.current();
  } while (inputs.next());

  assert(i==inputs.size());

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

//=======================================================================
//: Initialise the parameters from a text stream.
// The next non-ws character in the stream should be a '{'
// \verbatim
// {
//   sigma: 1.0  (optional, width of RBF)
//   power: 2.0  (optional, exponent of RBF)
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.

void clsfy_parzen_builder::config(std::istream &as)
{
 std::string s = mbl_parse_block(as);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  {
    sigma_= vul_string_atof(props.get_optional_property("sigma", "1.0"));
    power_= vul_string_atof(props.get_optional_property("k", "2.0"));
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
    "clsfy_parzen_builder::config", props, mbl_read_props_type());
}
