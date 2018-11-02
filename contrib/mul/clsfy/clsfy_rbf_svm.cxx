// This is mul/clsfy/clsfy_rbf_svm.cxx
// Copyright: (C) 2001 British Telecommunications plc
#include <string>
#include <iostream>
#include <cstdlib>
#include "clsfy_rbf_svm.h"
//:
// \file
// \brief Implement a RBF Support Vector Machine
// \author Ian Scott
// \date Jan 2001

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_math.h>

//=======================================================================

clsfy_rbf_svm::clsfy_rbf_svm() = default;

//=======================================================================

clsfy_rbf_svm::~clsfy_rbf_svm() = default;


//==================================================================

double clsfy_rbf_svm::kernel(const vnl_vector<double> &v1,
                             const vnl_vector<double> &v2) const
{
  return std::exp(gamma_*vnl_vector_ssd(v1, v2));
}

//=======================================================================

//: Classify the input vector.
// Returns 0 to indicate out of (or negative) class and one to
// indicate in class (or positive.)
unsigned clsfy_rbf_svm::classify(const vnl_vector<double> &input) const
{
  int n = supports_.size();
  double sum =- bias_;
  double upper_target = upper_target_;
  double lower_target = lower_target_;
  int i;
  for (i =0; i<n; i++)
  {
    const double l = lagrangians_[i];
    if (l <0) upper_target += l;
    else lower_target += l;
    sum += l * std::exp(gamma_*localEuclideanDistanceSq(input, supports_[i]));
    if (sum > upper_target) return 1u;
    else if (sum < lower_target) return 0u;
  }
  std::cerr << "ERROR: clsfy_rbf_svm::classify"
           << " Should not have reached here\n";
  std::abort();
  return 0u;
}

//=======================================================================

//: Log likelihood of being in class (binary classifiers only).
// class probability = std::exp(logL) / (1+std::exp(logL))
// This is not a strict log likelihood value, since SVMs do not give Bayesian
// outputs. However its properties fit the requirements of a log likelihood value.
double clsfy_rbf_svm::log_l(const vnl_vector<double> &input) const
{
  int n = supports_.size();
  double sum =0.0;
  for (int i =0; i<n; i++)
    sum += lagrangians_[i] * std::exp(gamma_*vnl_vector_ssd(input, supports_[i]));
  return sum - bias_;
}

//=======================================================================

//: Return the probability the input being in each class.
// output(i) i<<nClasses, contains the probability that the input
// is in class i;
// This are not strict probability values, since SVMs do not give Bayesian outputs. However
// their properties fit the requirements of a probability.
void clsfy_rbf_svm::class_probabilities(std::vector<double> &outputs,
                                        const vnl_vector<double> &input) const
{
  outputs.resize(1);
  double Likely = std::exp(log_l(input));
  if (Likely == vnl_huge_val(double()))
    outputs[0] = 1;
  else
    outputs[0] = Likely / (1+Likely);
  return;
}

//=======================================================================

//: Set the private target member values to the correct value.
void clsfy_rbf_svm::calculate_targets()
{
  upper_target_ = lower_target_=0;

  const unsigned n = supports_.size();
  for (unsigned i =0;i<n;i++)
  {
    const double l = lagrangians()[i];
    if (l < 0)  upper_target_ -= l;
    else        lower_target_ -= l;
  }
}


//=======================================================================

//: Set the internal values defining the classifier.
void clsfy_rbf_svm::set( const std::vector<vnl_vector<double> > &supportVectors,
                         const std::vector<double> &lagrangianAlphas,
                         const std::vector<unsigned> &labels,
                         double RBFWidth, double bias)
{
  unsigned int n = supportVectors.size();
  assert(n == lagrangianAlphas.size());
  assert(n == labels.size());
  supports_ = supportVectors;

  // pre-multiply Lagrangians with output labels.
  lagrangians_ = lagrangianAlphas;
  for (unsigned int i=0; i<n; i++)
    lagrangians_[i] *= (labels[i]?1:-1);

  gamma_ = -0.5/(RBFWidth*RBFWidth);
  bias_ = bias;
  calculate_targets();
}


//=======================================================================

std::string clsfy_rbf_svm::is_a() const
{
  return std::string("clsfy_rbf_svm");
}

//=======================================================================

bool clsfy_rbf_svm::is_class(std::string const& s) const
{
  return s == clsfy_rbf_svm::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

short clsfy_rbf_svm::version_no() const
{
  return 2;
}

//=======================================================================

clsfy_classifier_base* clsfy_rbf_svm::clone() const
{
  return new clsfy_rbf_svm(*this);
}

//=======================================================================

void clsfy_rbf_svm::print_summary(std::ostream& os) const
{
  os << vsl_indent() << "bias=" << bias_ << "  sigma=" << rbf_width()
     << "  nSupportVectors=" << n_support_vectors() << '\n'
     << vsl_indent() <<"  Starting targets are  " << upper_target_
     << ", " << lower_target_ << std::endl;
}

//=======================================================================

void clsfy_rbf_svm::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,bias_);
  vsl_b_write(bfs,gamma_);
  vsl_b_write(bfs,lagrangians_);
  vsl_b_write(bfs,supports_);
}

//=======================================================================

void clsfy_rbf_svm::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  double dummy;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
   case 1:
    std::cerr << "WARNING: clsfy_rbf_svm::b_read().\n"
             << "Version 1 shouldn't really be loaded into this class.\n";

    vsl_b_read(bfs,bias_);
    vsl_b_read(bfs,gamma_);
    vsl_b_read(bfs,lagrangians_);
    vsl_b_read(bfs,supports_);
    vsl_b_read(bfs,dummy);
    vsl_b_read(bfs,dummy);
    calculate_targets();
    break;
   case 2:
    vsl_b_read(bfs,bias_);
    vsl_b_read(bfs,gamma_);
    vsl_b_read(bfs,lagrangians_);
    vsl_b_read(bfs,supports_);
    calculate_targets();
    break;
   default:
    std::cerr << "I/O ERROR: clsfy_rbf_svm::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
