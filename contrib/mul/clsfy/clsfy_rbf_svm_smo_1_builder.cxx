//  Copyright: (C) 2001 British Telecommunications plc.

//:
// \file
// \brief Implement an interface to SMO algorithm SVM builder and additional logic
// \author Ian Scott
// \date Dec 2001

//=======================================================================

#include <clsfy/clsfy_smo_1.h>
#include <clsfy/clsfy_rbf_svm_smo_1_builder.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vcl_limits.h>
#include <vcl_queue.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_brent.h>

#include <vsl/vsl_indent.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <mbl/mbl_mz_random.h>
#include <vcl_cassert.h>

#include <mbl/mbl_data_array_wrapper.h>

//=======================================================================

clsfy_rbf_svm_smo_1_builder::clsfy_rbf_svm_smo_1_builder()
{
  boundC_ = 0;
  rbf_width_ = 1.0;
}

//=======================================================================

inline int class_to_svm_target (unsigned v) {return v==1?1:-1;};

//=======================================================================
//: Build classifier from data
// returns the training error, or +INF if there is an error.
double clsfy_rbf_svm_smo_1_builder::build(clsfy_classifier_base& classifier,
  mbl_data_wrapper<vnl_vector<double> >& inputs,
  const vcl_vector<unsigned> &outputs) const
{
  inputs.reset();
//const unsigned int nDims = inputs.current().size(); // unused variable
  const unsigned int nSamples = inputs.size();
  assert(outputs.size() == nSamples);
  assert(*vcl_max_element(outputs.begin(), outputs.end()) <= 1);

  assert(classifier.is_a() == "clsfy_rbf_svm");
  clsfy_rbf_svm &svm = static_cast<clsfy_rbf_svm &>(classifier);

  clsfy_smo_1_rbf svAPI;
  vcl_vector<int> targets(nSamples);
  vcl_transform(outputs.begin(), outputs.end(),
    targets.begin(), class_to_svm_target);

  svAPI.set_data(inputs, targets);


  // Set the SVM solver parameters
  svAPI.set_C(boundC_);
  svAPI.set_gamma(1.0/(2.0*rbf_width_*rbf_width_));
  // Solve the SVM
  svAPI.calc();


  // Get the SVM description, and build an SVM machine
  {
    vcl_vector<vnl_vector<double> > supportVectors;
    const vnl_vecd &allAlphas = svAPI.lagrange_mults();
    vcl_vector<double> alphas;
    vcl_vector<unsigned> labels;
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
                                          const vcl_vector<unsigned> &outputs) const
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

vcl_string clsfy_rbf_svm_smo_1_builder::is_a() const
{
  return vcl_string("clsfy_rbf_svm_smo_1_builder");
}

//=======================================================================

bool clsfy_rbf_svm_smo_1_builder::is_class(vcl_string const& s) const
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

void clsfy_rbf_svm_smo_1_builder::print_summary(vcl_ostream& os) const
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
    vcl_cerr << "I/O ERROR: clsfy_rbf_svm_smo_1_builder::b_read(vsl_b_istream&) \n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
