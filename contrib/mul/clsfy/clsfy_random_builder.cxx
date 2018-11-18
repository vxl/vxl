// This is mul/clsfy/clsfy_random_builder.cxx
// Copyright (c) 2001: British Telecommunications plc
//:
// \file
// \brief Implement a random classifier builder
// \author Ian Scott
// \date 2001-10-08

#include <iostream>
#include <string>
#include "clsfy_random_builder.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <clsfy/clsfy_random_classifier.h>
#include <vnl/vnl_c_vector.h>

//=======================================================================

clsfy_random_builder::clsfy_random_builder():
confidence_(0.0)
{}

//=======================================================================

short clsfy_random_builder::version_no() const
{
  return 1;
}

//=======================================================================

std::string clsfy_random_builder::is_a() const
{
  return std::string("clsfy_random_builder");
}

//=======================================================================

bool clsfy_random_builder::is_class(std::string const& s) const
{
  return s == clsfy_random_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_random_builder::clone() const
{
  return new clsfy_random_builder(*this);
}

//=======================================================================

void clsfy_random_builder::print_summary(std::ostream& os) const
{
  os << "confidence = " << confidence_;
}

//=======================================================================

void clsfy_random_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, confidence_);
}

//=======================================================================

void clsfy_random_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, confidence_);
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_random_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_random_builder::build(clsfy_classifier_base& model,
                                   mbl_data_wrapper<vnl_vector<double> >& inputs,
                                   unsigned nClasses,
                                   const std::vector<unsigned> &outputs) const
{
  const unsigned n = outputs.size();
  assert(model.is_class("clsfy_random_classifier"));
  auto &randclass = (clsfy_random_classifier&) model;

  if (nClasses==1) nClasses=2;

  assert (n>0);

  std::vector<unsigned> freqs(nClasses);
  for (unsigned i=0; i < n; ++i)
  {
    assert (outputs[i] < nClasses);
    freqs[outputs[i]] ++;
  }

  auto sum = (double)(vnl_c_vector<unsigned>::sum(&freqs.front(), nClasses));
  std::vector<double> probs(nClasses);
  for (unsigned i=0; i < nClasses; ++i)
    probs[i] = freqs[i] / sum;
  randclass.set_probs(probs);

  assert (inputs.size() > 0);
  inputs.reset();
  randclass.set_n_dims(inputs.current().size());
  randclass.set_confidence(confidence_);
  return clsfy_test_error(model, inputs, outputs);
}

//=======================================================================

double clsfy_random_builder::confidence() const
{
  return confidence_;
}

//=======================================================================

void clsfy_random_builder::set_confidence(double confidence)
{
  confidence_ = confidence;
}

//=======================================================================

//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_random_builder::new_classifier() const
{
  return new clsfy_random_classifier();
}
