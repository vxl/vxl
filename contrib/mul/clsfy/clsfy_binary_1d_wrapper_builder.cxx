// Copyright: (C) 2009 Imorphics PLC
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "clsfy_binary_1d_wrapper_builder.h"
//:
// \file
// \brief Wrap a builder_1d in general builder_base derivative.
// \author Ian Scott
// \date 2 Sep 2009

//=======================================================================

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>
#include <clsfy/clsfy_classifier_1d.h>

// Dflt ctor
clsfy_binary_1d_wrapper_builder::clsfy_binary_1d_wrapper_builder():
  builder_1d_(new clsfy_binary_threshold_1d_builder)
{}

//: Create a new untrained linear classifier with binary output
  clsfy_classifier_base* clsfy_binary_1d_wrapper_builder::new_classifier() const
{
  std::unique_ptr<clsfy_classifier_1d> c_1d(builder_1d_->new_classifier());

  clsfy_binary_1d_wrapper classifier;
  classifier.set_classifier_1d(*c_1d);
  return classifier.clone();
}


//=======================================================================

std::string clsfy_binary_1d_wrapper_builder::is_a() const
{
  return std::string("clsfy_binary_1d_wrapper_builder");
}

//=======================================================================

bool clsfy_binary_1d_wrapper_builder::is_class(std::string const& s) const
{
  return s == clsfy_binary_1d_wrapper_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

void clsfy_binary_1d_wrapper_builder::print_summary(std::ostream& os) const
{
  os << "Underlying 1d builder: "; vsl_print_summary(os, builder_1d_);
}

//=======================================================================

//: Build a multi layer perceptron classifier, with the given data.
double clsfy_binary_1d_wrapper_builder::build(
  clsfy_classifier_base &classifier,
  mbl_data_wrapper<vnl_vector<double> > &inputs,
  const std::vector<unsigned> &outputs) const
{
  assert(outputs.size() == inputs.size());
  assert(* std::max_element(outputs.begin(), outputs.end()) <= 1);
  assert(classifier.is_class("clsfy_binary_1d_wrapper"));

  auto &c_wrap = (clsfy_binary_1d_wrapper &) classifier;

  std::unique_ptr<clsfy_classifier_1d> c_1d(builder_1d_->new_classifier());

  vnl_vector<double> inputs_1d(inputs.size());
  unsigned i=0;
  do
  {
    inputs_1d(i++)=inputs.current()(0);
  } while (inputs.next());

  assert (i=inputs.size());
  vnl_vector<double> wts(inputs.size(), 1.0/inputs.size());
  builder_1d_->build(*c_1d, inputs_1d, wts, outputs);

  c_wrap.set_classifier_1d(*c_1d);

  return clsfy_test_error(classifier, inputs, outputs);
}


//=======================================================================


//: Build a linear classifier, with the given data.
// Return the mean error over the training set.
// n_classes must be 1.
double clsfy_binary_1d_wrapper_builder::build(
  clsfy_classifier_base &classifier, mbl_data_wrapper<vnl_vector<double> > &inputs,
  unsigned n_classes, const std::vector<unsigned> &outputs) const
{
  assert (n_classes == 1);
  return build(classifier, inputs, outputs);
}

//=======================================================================

void clsfy_binary_1d_wrapper_builder::b_write(vsl_b_ostream &bfs) const
{
  constexpr short version_no = 1;
  vsl_b_write(bfs, version_no);
  vsl_b_write(bfs, builder_1d_);
}

//=======================================================================

void clsfy_binary_1d_wrapper_builder::b_read(vsl_b_istream &bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs, version);
  switch (version)
  {
    case 1:
     vsl_b_read(bfs, builder_1d_);
     break;
    default:
      std::cerr << "I/O ERROR: clsfy_binary_1d_wrapper_builder::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}


//=======================================================================
//: Initialise the parameters from a text stream.
// The next non-ws character in the stream should be a '{'
// \verbatim
// {
//   builder_1d: clsfy_binary_threshold_1d_builder  (required, underlying builder)
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void clsfy_binary_1d_wrapper_builder::config(std::istream &as)
{
 std::string s = mbl_parse_block(as);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  {
    std::stringstream ss2(props.get_required_property("builder_1d"));
    std::unique_ptr<clsfy_builder_1d> b_1d =
      clsfy_builder_1d::new_builder(ss2);
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
    "clsfy_binary_1d_wrapper_builder::config", props, mbl_read_props_type());
}
