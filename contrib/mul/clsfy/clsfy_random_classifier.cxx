// This is mul/clsfy/clsfy_random_classifier.cxx
// Copyright (c) 2001: British Telecommunications plc

//:
// \file
// \brief  Implement a random classifier
// \author iscott
// \date   Tue Oct  9 10:21:59 2001

#include <iostream>
#include <string>
#include "clsfy_random_classifier.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <clsfy/clsfy_classifier_base.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_math.h>

//=======================================================================

clsfy_random_classifier::clsfy_random_classifier():
confidence_(0.0), n_dims_(0u)
{
}

//=======================================================================

std::string clsfy_random_classifier::is_a() const
{
  return std::string("clsfy_random_classifier");
}

//=======================================================================

bool clsfy_random_classifier::is_class(std::string const& s) const
{
  return s == clsfy_random_classifier::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

clsfy_classifier_base* clsfy_random_classifier::clone() const
{
  return new clsfy_random_classifier(*this);
}

//=======================================================================

    // required if data is present in this base class
void clsfy_random_classifier::print_summary(std::ostream& os) const
{
  os << "Prior probs = "; vsl_print_summary(os, probs_);
  os << ", confidence = " << confidence_<<'\n';
}

//=======================================================================

static short version_no = 1;

  // required if data is present in this base class
void clsfy_random_classifier::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no);
  vsl_b_write(bfs, probs_);
  vsl_b_write(bfs, confidence_);
  vsl_b_write(bfs, n_dims_);
}

//=======================================================================

  // required if data is present in this base class
void clsfy_random_classifier::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs, version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, probs_);
    calc_min_to_win();
    vsl_b_read(bfs, confidence_);
    vsl_b_read(bfs, n_dims_);
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_random_classifier&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

double clsfy_random_classifier::confidence() const
{
  return confidence_;
}

//=======================================================================

void clsfy_random_classifier::set_confidence(double confidence)
{
  assert(confidence >= 0.0);
  confidence_ = confidence;
}

//=======================================================================

const std::vector<double> & clsfy_random_classifier::probs() const
{
  return probs_;
}

//=======================================================================

void clsfy_random_classifier::calc_min_to_win()
{
  const unsigned n = probs_.size();
  min_to_win_.resize(n);
  for (unsigned i=0; i<n; ++i)
  {
    double maxval = -1;
    for (unsigned j=0; j<n; ++j)
    {
      if (j==i) continue;
      if (probs_[j] > maxval)
        maxval = probs_[j];
    }
    min_to_win_[i] = maxval - probs_[i] + vnl_math::sqrteps;
  }
}

//=======================================================================
typedef vnl_c_vector<double> cvd;

void clsfy_random_classifier::set_probs(const std::vector<double> &probs)
{
  probs_ = probs;
  const unsigned n = probs_.size();
  assert(n > 1);

  double * const p = &probs_.front();

  cvd::scale(p, p, n, 1.0/cvd::sum(p, n));

  calc_min_to_win();
}

//=======================================================================

void clsfy_random_classifier::set_n_dims(unsigned n_dims)
{
  n_dims_ = n_dims;
}

//=======================================================================

//: The dimensionality of input vectors.
unsigned clsfy_random_classifier::n_dims() const
{
  return n_dims_;
}

//=======================================================================

//: The number of possible output classes.
unsigned clsfy_random_classifier::n_classes() const
{
  return probs_.size()==2?1:probs_.size();
}

//=======================================================================

//: Return the probability the input being in each class.
// output(i) i<nClasses, contains the probability that the input is in class i
void clsfy_random_classifier::class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const
{
  if (last_inputs_ != input)
  {
    last_inputs_ = input;
    unsigned i=0;
    double x=rng_.drand64() -probs_[0];
    while (x>=0)
      x-= probs_[++i];

    const unsigned n = probs_.size();
    assert(i<n);

    last_outputs_ = probs_;
    last_outputs_[i] += min_to_win_[i] + vnl_math::abs(rng_.normal()) * confidence_;

    double * const p = &last_outputs_[0];
    cvd::scale(p, p, n, 1.0/cvd::sum(p, n));
  }

// Convert a two-class output into a binary output
  if (last_outputs_.size() == 2)
  {
    outputs.resize(1);
    outputs[0] = last_outputs_[1];
  }
  else
    outputs = last_outputs_;
}

//=======================================================================

//: Log likelihood of being in class (binary classifiers only)
// class probability = 1 / (1+exp(-log_l))
// Operation of this method is undefined for multiclass classifiers.
double clsfy_random_classifier::log_l(const vnl_vector<double> &input) const
{
  assert (n_classes() == 1);
  std::vector<double> prob(1);
  class_probabilities(prob, input);
  return std::log(prob[0]/(1-prob[0]));
}

//=======================================================================

void clsfy_random_classifier::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}
