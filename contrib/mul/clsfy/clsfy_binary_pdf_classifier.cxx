// This is mul/clsfy/clsfy_binary_pdf_classifier.cxx
//  Copyright: (C) 2000 British Telecommunications PLC
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include "clsfy_binary_pdf_classifier.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_binary_loader.h>
#include <cassert>

//=======================================================================

void clsfy_binary_pdf_classifier::deleteStuff()
{
  delete pdf_;
  pdf_ = nullptr;
}

//=======================================================================

//: Classify the input vector
// Returns either class1 (Inside PDF mode) or class 0 (Outside PDF mode).
unsigned clsfy_binary_pdf_classifier::classify(const vnl_vector<double> &input) const
{
  assert(pdf_!=nullptr);

  if (pdf_->log_p(input) >= log_prob_limit_)
    return 1;
  else
    return 0;
}

//=======================================================================

//: Return the probability the input being in each class P(class|data).
// output(i) i<n_classes, contains the probability that the input is in class i
//
void clsfy_binary_pdf_classifier::class_probabilities(
          std::vector<double> &outputs,
          const vnl_vector<double> &input)  const
{
  // likelihood = P(input|InClass) / prob_limit_
  double likelihood= std::exp(log_l(input));
  outputs.resize(1);
  outputs[0] = likelihood / (1 + likelihood);
}

//=======================================================================

//: Log likelihood of being in class 0.  log(P(class=0|data))
// This function is intended for use in binary classifiers only. It is
// related to the class 0 probability as follows,
// P(class=0|data) = exp(logL) / (1+exp(logL)).
// Don't forget that P(X|data) is a density and so the result can be
// greater than 1.0 or less than 0.0, (or indeed between 0.0 and 1.0).
double clsfy_binary_pdf_classifier::log_l(const vnl_vector<double> &input) const
{
  assert(pdf_!=nullptr);

  // likelihood = P(input|InClass) / prob_limit_
  return pdf_->log_p(input) - log_prob_limit_;
}

//=======================================================================

std::string clsfy_binary_pdf_classifier::is_a() const
{
  return std::string("clsfy_binary_pdf_classifier");
}

//=======================================================================

bool clsfy_binary_pdf_classifier::is_class(std::string const& s) const
{
  return s == clsfy_binary_pdf_classifier::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

short clsfy_binary_pdf_classifier::version_no() const
{
  return 1;
}

//=======================================================================

// required if data is present in this class
void clsfy_binary_pdf_classifier::print_summary(std::ostream& os) const
{
  os << "log Probability limit, " << log_prob_limit_ << " ,PDF " << pdf_;
}

//=======================================================================

clsfy_classifier_base* clsfy_binary_pdf_classifier::clone() const
{
  return new clsfy_binary_pdf_classifier(*this);
}

//=======================================================================

clsfy_binary_pdf_classifier& clsfy_binary_pdf_classifier::operator=(const clsfy_binary_pdf_classifier& classifier)
{
  if (&classifier==this) return *this;

  clsfy_classifier_base::operator=(classifier);

  deleteStuff();

  if (classifier.pdf_)
    pdf_ = classifier.pdf_->clone();

  log_prob_limit_ = classifier.log_prob_limit_;

  return *this;
}


//=======================================================================

void clsfy_binary_pdf_classifier::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,log_prob_limit_);
  vsl_b_write(bfs,pdf_);
}

//=======================================================================

void clsfy_binary_pdf_classifier::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,log_prob_limit_);
      vsl_b_read(bfs,pdf_);
      break;
    default:
      std::cerr << "I/O ERROR: clsfy_binary_pdf_classifier::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << "\n";
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}
