// This is mul/clsfy/clsfy_rbf_svm.cxx
// Copyright: (C) 2001 British Telecommunications plc
#include "clsfy_rbf_svm.h"
//:
// \file
// \brief Implement a RBF Support Vector Machine
// \author Ian Scott
// \date Jan 2001

#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_math.h>

//=======================================================================

clsfy_rbf_svm::clsfy_rbf_svm()
{
}

//=======================================================================

clsfy_rbf_svm::~clsfy_rbf_svm()
{
}


//==================================================================

double clsfy_rbf_svm::kernel(const vnl_vector<double> &v1,
                             const vnl_vector<double> &v2) const
{
  return vcl_exp(gamma_*vnl_vector_ssd(v1, v2));
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
    sum += l * vcl_exp(gamma_*localEuclideanDistanceSq(input, supports_[i]));
    if (sum > upper_target) return 1u;
    else if (sum < lower_target) return 0u;
  }
  vcl_cerr << "ERROR: clsfy_rbf_svm::classify"
           << " Should not have reached here" <<vcl_endl;
  vcl_abort();
  return 0u;
}

//=======================================================================

//: Log likelihood of being in class (binary classifiers only).
// class probability = vcl_exp(logL) / (1+vcl_exp(logL))
// This is not a strict log likelihood value, since SVMs do not give Bayesian
// outputs. However its properties fit the requirements of a log likelihood value.
double clsfy_rbf_svm::log_l(const vnl_vector<double> &input) const
{
  int n = supports_.size();
  double sum =0.0;
  for (int i =0; i<n; i++)
    sum += lagrangians_[i] * vcl_exp(gamma_*vnl_vector_ssd(input, supports_[i]));
  return sum - bias_;
}

//=======================================================================

//: Return the probability the input being in each class.
// output(i) i<<nClasses, contains the probability that the input
// is in class i;
// This are not strict probability values, since SVMs do not give Bayesian outputs. However
// their properties fit the requirements of a probability.
void clsfy_rbf_svm::class_probabilities(vcl_vector<double> &outputs,
                                        const vnl_vector<double> &input) const
{
  outputs.resize(1);
  double Likely = vcl_exp(log_l(input));
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
void clsfy_rbf_svm::set( const vcl_vector<vnl_vector<double> > &supportVectors,
                         const vcl_vector<double> &lagrangianAlphas,
                         const vcl_vector<unsigned> &labels,
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

vcl_string clsfy_rbf_svm::is_a() const
{
  return vcl_string("clsfy_rbf_svm");
}

//=======================================================================

bool clsfy_rbf_svm::is_class(vcl_string const& s) const
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

void clsfy_rbf_svm::print_summary(vcl_ostream& os) const
{
  os << vsl_indent() << "bias=" << bias_ << "  sigma=" << rbf_width();
  os << "  nSupportVectors=" << n_support_vectors() << vcl_endl;
  os << vsl_indent() <<"  Starting targets are  " << upper_target_;
  os << ", " << lower_target_ << vcl_endl;
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

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    double dummy;
    vcl_cerr << "WARNING: clsfy_rbf_svm::b_read().\nVersion" <<
      "1 shouldn't really be loaded into this class." << vcl_endl;

    vsl_b_read(bfs,bias_);
    vsl_b_read(bfs,gamma_);
    vsl_b_read(bfs,lagrangians_);
    vsl_b_read(bfs,supports_);
    vsl_b_read(bfs,dummy);
    vsl_b_read(bfs,dummy);
    calculate_targets();
    break;
  case (2):
    vsl_b_read(bfs,bias_);
    vsl_b_read(bfs,gamma_);
    vsl_b_read(bfs,lagrangians_);
    vsl_b_read(bfs,supports_);
    calculate_targets();
    break;
  default:
    vcl_cerr << "I/O ERROR: clsfy_rbf_svm::b_read(vsl_b_istream&) \n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
