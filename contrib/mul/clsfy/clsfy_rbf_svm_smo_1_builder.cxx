// This is mul/clsfy/clsfy_rbf_svm_smo_1_builder.cxx
// Copyright: (C) 2001 British Telecommunications plc.
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "clsfy_rbf_svm_smo_1_builder.h"
//:
// \file
// \brief Implement an interface to SMO algorithm SVM builder and additional logic
// \author Ian Scott
// \date Dec 2001

//=======================================================================

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vul/vul_string.h>

#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <clsfy/clsfy_smo_1.h>

//=======================================================================

inline int class_to_svm_target (unsigned v) {return v==1?1:-1;}

//=======================================================================
//: Build classifier from data
// returns the training error, or +INF if there is an error.
double clsfy_rbf_svm_smo_1_builder::build(clsfy_classifier_base& classifier,
                                          mbl_data_wrapper<vnl_vector<double> >& inputs,
                                          const std::vector<unsigned> &outputs) const
{
  inputs.reset();
//const unsigned int nDims = inputs.current().size(); // unused variable
  const unsigned int nSamples = inputs.size();
  assert(outputs.size() == nSamples);
  assert(*std::max_element(outputs.begin(), outputs.end()) <= 1);

  assert(classifier.is_class("clsfy_rbf_svm"));
  auto &svm = static_cast<clsfy_rbf_svm &>(classifier);

  clsfy_smo_1_rbf svAPI;
  std::vector<int> targets(nSamples);
  std::transform(outputs.begin(), outputs.end(),
                targets.begin(), class_to_svm_target);

  svAPI.set_data(inputs, targets);


  // Set the SVM solver parameters
  svAPI.set_C(boundC_);
  svAPI.set_gamma(1.0/(2.0*rbf_width_*rbf_width_));
  // Solve the SVM
  svAPI.calc();


  // Get the SVM description, and build an SVM machine
  {
    std::vector<vnl_vector<double> > supportVectors;
    const vnl_vector<double> &allAlphas = svAPI.lagrange_mults();
    std::vector<double> alphas;
    std::vector<unsigned> labels;
    for (unsigned i=0; i<nSamples; ++i)
      if (allAlphas[i]!=0.0)
      {
        alphas.push_back(allAlphas[i]);
        labels.push_back(outputs[i]);
        inputs.set_index(i);
        supportVectors.push_back(inputs.current());
      }
    svm.set(supportVectors, alphas, labels, rbf_width_, svAPI.bias());
  }

  return svAPI.error_rate();
}

//=======================================================================
//: Build classifier from data.
// returns the training error, or +INF if there is an error.
// nClasses must be 1.
double clsfy_rbf_svm_smo_1_builder::build(clsfy_classifier_base& classifier,
                                          mbl_data_wrapper<vnl_vector<double> >& inputs,
                                          unsigned nClasses,
                                          const std::vector<unsigned> &outputs) const
{
  assert(nClasses == 1);
  return build(classifier, inputs, outputs);
}

//=======================================================================

double clsfy_rbf_svm_smo_1_builder::rbf_width() const
{
  return rbf_width_;
}

//=======================================================================

void clsfy_rbf_svm_smo_1_builder::set_rbf_width(double rbf_width)
{
  rbf_width_ = rbf_width;
}
//=======================================================================

std::string clsfy_rbf_svm_smo_1_builder::is_a() const
{
  return std::string("clsfy_rbf_svm_smo_1_builder");
}

//=======================================================================

bool clsfy_rbf_svm_smo_1_builder::is_class(std::string const& s) const
{
  return s == clsfy_rbf_svm_smo_1_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

short clsfy_rbf_svm_smo_1_builder::version_no() const
{
  return 1;
}

//=======================================================================

clsfy_builder_base* clsfy_rbf_svm_smo_1_builder::clone() const
{
  return new clsfy_rbf_svm_smo_1_builder(*this);
}

//=======================================================================

void clsfy_rbf_svm_smo_1_builder::print_summary(std::ostream& os) const
{
  // os << data_; // example of data output
  os << "RBF width = " << rbf_width_ << ", bounds = " << boundC_;
}

//=======================================================================

void clsfy_rbf_svm_smo_1_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,boundC_);
  vsl_b_write(bfs,rbf_width_);
}

//=======================================================================

void clsfy_rbf_svm_smo_1_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,boundC_);
    vsl_b_read(bfs,rbf_width_);
    break;
  default:
    std::cerr << "I/O ERROR: clsfy_rbf_svm_smo_1_builder::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//=======================================================================
//: Initialise the parameters from a text stream.
// The next non-ws character in the stream should be a '{'
// \verbatim
// {
//   boundC: 3  (default 0 meaning no bound) Upper bound on the Lagrange multiplies.
//              Smaller non-zero values result in a softening of the boundary.
//
//   rbf_width: 3.0  (required) - A good guess is the mean euclidean distance
//                    to every examples nearest neighbour.
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void clsfy_rbf_svm_smo_1_builder::config(std::istream &as)
{
 std::string s = mbl_parse_block(as);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  {
    boundC_= vul_string_atof(props.get_optional_property("boundC", "0.0"));
    rbf_width_= vul_string_atof(props.get_optional_property("rbf_width", "0.0"));
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
    "clsfy_rbf_svm_smo_1_builder::config", props, mbl_read_props_type());
}
