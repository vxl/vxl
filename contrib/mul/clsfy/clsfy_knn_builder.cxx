// This is mul/clsfy/clsfy_knn_builder.cxx
// Copyright (c) 2001: British Telecommunications plc
//:
// \file
// \brief Implement a knn classifier builder
// \author Ian Scott
// \date 2001-10-07

#include <iostream>
#include <sstream>
#include <string>
#include "clsfy_knn_builder.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>

//=======================================================================

clsfy_knn_builder::clsfy_knn_builder():
k_(1)
{
}


//=======================================================================

short clsfy_knn_builder::version_no() const
{
  return 1;
}

//=======================================================================

std::string clsfy_knn_builder::is_a() const
{
  return std::string("clsfy_knn_builder");
}

//=======================================================================

bool clsfy_knn_builder::is_class(std::string const& s) const
{
  return s == clsfy_knn_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_knn_builder::clone() const
{
  return new clsfy_knn_builder(*this);
}

//=======================================================================

void clsfy_knn_builder::print_summary(std::ostream& os) const
{
  os << "k = " << k_;
}

//=======================================================================

void clsfy_knn_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, k_);
  std::cerr << "clsfy_knn_builder::b_write() NYI\n";
}

//=======================================================================

void clsfy_knn_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, k_);
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_knn_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_knn_builder::build(clsfy_classifier_base& model,
                                mbl_data_wrapper<vnl_vector<double> >& inputs,
                                unsigned /* nClasses */,
                                const std::vector<unsigned> &outputs) const
{
  assert(model.is_class("clsfy_k_nearest_neighbour")); // equiv to dynamic_cast<> != 0
  assert(inputs.size()==outputs.size());

  auto &knn = (clsfy_k_nearest_neighbour&) model;

  std::vector<vnl_vector<double> > vin(inputs.size());

  inputs.reset();
  unsigned i=0;
  do
  {
    vin[i++] = inputs.current();
  } while (inputs.next());

  assert(i==inputs.size());

  knn.set(vin, outputs);
  knn.set_k(k_);
  return clsfy_test_error(model, inputs, outputs);
}

//=======================================================================

unsigned clsfy_knn_builder::k() const
{
  return k_;
}

//=======================================================================

void clsfy_knn_builder::set_k(unsigned k)
{
  k_ = k;
}

//=======================================================================
//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_knn_builder::new_classifier() const
{
  return new clsfy_k_nearest_neighbour();
}

//=======================================================================
//: Initialise the parameters from a text stream.
// The next non-ws character in the stream should be a '{'
// \verbatim
// {
//   k: 3  (default 1)
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void clsfy_knn_builder::config(std::istream &as)
{
 std::string s = mbl_parse_block(as);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  {
    k_= vul_string_atoi(props.get_optional_property("k", "1"));
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
    "clsfy_knn_builder::config", props, mbl_read_props_type());
}
