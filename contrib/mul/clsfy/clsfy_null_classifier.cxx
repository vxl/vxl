// This is mul/clsfy/clsfy_null_classifier.cxx
// Copyright (c) 2009: Imorphics plc

//:
// \file
// \brief  Implement a null classifier
// \author iscott
// \date    25 Aug 2009

#include <iostream>
#include <string>
#include <algorithm>
#include "clsfy_null_classifier.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>


//=======================================================================

std::string clsfy_null_classifier::is_a() const
{
  return std::string("clsfy_null_classifier");
}

//=======================================================================

bool clsfy_null_classifier::is_class(std::string const& s) const
{
  return s == clsfy_null_classifier::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

clsfy_classifier_base* clsfy_null_classifier::clone() const
{
  return new clsfy_null_classifier(*this);
}

//=======================================================================

void clsfy_null_classifier::print_summary(std::ostream& os) const
{
  os << "n_dims: " << n_dims_ << " default class: " << default_class_;
}

//=======================================================================
void clsfy_null_classifier::b_write(vsl_b_ostream& bfs) const
{
  constexpr short version_no = 1;
  vsl_b_write(bfs, version_no);
  vsl_b_write(bfs, n_dims_);
  vsl_b_write(bfs, default_class_);
}

//=======================================================================

  // required if data is present in this base class
void clsfy_null_classifier::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs, version);
  switch (version)
  {
  case 1:
    vsl_b_read(bfs, n_dims_);
    vsl_b_read(bfs, default_class_);
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_null_classifier&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

//: The number of possible output classes.
unsigned clsfy_null_classifier::n_classes() const
{
  return std::max(1u,default_class_);
}

//=======================================================================

//: Return the probability the input being in each class.
// output(i) i<nClasses, contains the probability that the input is in class i
void clsfy_null_classifier::class_probabilities(std::vector<double> &outputs, const vnl_vector<double> & /*input*/) const
{
  unsigned n=n_classes();
  outputs.resize(n);
  for (unsigned i=0; i<n; ++i)
    outputs[i] = default_class_?1.0:0.0;
}

//=======================================================================

//: Log likelihood of being in class (binary classifiers only)
// class probability = 1 / (1+exp(-log_l))
// Operation of this method is undefined for multiclass classifiers.
double clsfy_null_classifier::log_l(const vnl_vector<double> & /*input*/) const
{
  return default_class_?1.0:-1.0 * vnl_huge_val(double());
}
