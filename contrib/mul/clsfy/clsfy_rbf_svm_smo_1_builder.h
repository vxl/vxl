#ifndef clsfy_rbf_svm_smo_1_builder_h_
#define clsfy_rbf_svm_smo_1_builder_h_

// Copyright: (C) 2001 British Telecommunications plc.

//:
// \file
// \brief Describe an interface to an SMO SVM builder and additional logic
// \author Ian Scott
// \date Dec 2001

#include <clsfy/clsfy_builder_base.h>
#include <clsfy/clsfy_rbf_svm.h>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_data_wrapper.h>


//: Base for classes to build clsfy_classifier_base objects
class clsfy_rbf_svm_smo_1_builder : public clsfy_builder_base
{
  //: C, the upper bound on the Lagrange multipliers.
  // 0 means no (or infinite) upper bound.
  double boundC_;

  //: Width of Gaussian function
  double rbf_width_;

  //: Reorder the vectors in a classifier to make it more efficient
  // The members of outputs() should be 0 or 1.
  // The default value of ratioNumberPos2NegSamples will be 
  // the ratio of number of positive samples to negative samples found in the training set
  void find_efficient_sv_ordering(
    clsfy_classifier_base& classifier,
    mbl_data_wrapper<vnl_vector<double> >& inputs,
    const vcl_vector<unsigned> &outputs) const;

public:

  //: Dflt ctor
  clsfy_rbf_svm_smo_1_builder();

  //: Destructor
  virtual ~clsfy_rbf_svm_smo_1_builder() {}

  //: Create empty model
  virtual clsfy_classifier_base* new_classifier() const{return new clsfy_rbf_svm();}

  //: Build model from data
  // returns the empirical error, or +INF if there is an error.
  // nclasses should be 1. The members of outputs() should be 0 or 1.
  // An appropriate RBF width is chosen by the algorithm.
  virtual double build(clsfy_classifier_base& classifier,
    mbl_data_wrapper<vnl_vector<double> >& inputs,
    unsigned nClasses,
    const vcl_vector<unsigned> &outputs) const;

  //: Build a classifier from data.
  // returns the empirical error rate, or +INF if there is an error.
  // The members of outputs() should be 0 or 1.
  virtual double build(clsfy_classifier_base& classifier,
    mbl_data_wrapper<vnl_vector<double> >& inputs,
    const vcl_vector<unsigned> &outputs) const;

  //: Radius of Gaussian function
  double rbf_width() const;

  //: Set the Radius of Gaussian function
  void set_rbf_width(double);

  //: Set C, the upper bound on the Lagrange multipliers
  // 0 means no (or infinite) upper bound and is the default.
  // A finite value effectively allows the training data to encroach into
  // the separating margin. Smaller C -> smoother boundary
  void set_bound_on_multipliers(double C)
  { boundC_ = C; }


  //: The upper bound on the Lagrange multipliers
  // 0 means no (or infinite) upper bound.
  // A finite value effectively allows the training data to encroach into
  // the separating margin. Smaller C -> smoother boundary
  double bound_on_multipliers() const
  { return boundC_; }

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Return the class's IO version number
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // clsfy_rbf_svm_smo_1_builder_h_
