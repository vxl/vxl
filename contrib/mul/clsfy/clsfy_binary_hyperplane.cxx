// This is mul/clsfy/clsfy_binary_hyperplane.cxx
// Copyright: (C) 2000 British Telecommunications PLC
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include "clsfy_binary_hyperplane.h"
//:
// \file
// \brief Implement a binary linear classifier
// \author Ian Scott
// \date 2000-05-26
//
//=======================================================================

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>

//=======================================================================

//: Classify the input vector.
// Returns either 0 (for negative class) or 1 (for positive class)
unsigned clsfy_binary_hyperplane::classify(const vnl_vector<double> &input) const
{
  if (dot_product(input, weights_) - bias_ > 0.0) return 1;
  else return 0;
}


//=======================================================================

//: Find the posterior probability of the input being in the positive class.
// The result is outputs(0)
void clsfy_binary_hyperplane::class_probabilities(std::vector<double> &outputs,
                                                  const vnl_vector<double> &input) const
{
  outputs.resize(1);
  outputs[0] = 1.0 / (1.0 + std::exp(-log_l(input)));
}


//=======================================================================

//: Log likelihood of being in the positive class.
// Class probability = 1 / (1+exp(-log_l))
double clsfy_binary_hyperplane::log_l(const vnl_vector<double> &input) const
{
  return dot_product(input, weights_) - bias_;
}


//=======================================================================


std::string clsfy_binary_hyperplane::is_a() const
{
  return std::string("clsfy_binary_hyperplane");
}

//=======================================================================

bool clsfy_binary_hyperplane::is_class(std::string const& s) const
{
  return s == clsfy_binary_hyperplane::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

//:
// required if data is present in this class
void clsfy_binary_hyperplane::print_summary(std::ostream& os) const
{
  os << "bias: " << bias_ << "   weights: ";
  vsl_print_summary(os, weights_);
}

//=======================================================================

short clsfy_binary_hyperplane::version_no() const
{
  return 1;
}

//=======================================================================

void clsfy_binary_hyperplane::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,weights_);
  vsl_b_write(bfs,bias_);
}

//=======================================================================

void clsfy_binary_hyperplane::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,weights_);
      vsl_b_read(bfs,bias_);
      break;
    default:
      std::cerr << "I/O ERROR: clsfy_binary_hyperplane::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}
