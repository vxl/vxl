// This is mul/clsfy/clsfy_rbf_svm_smo_1_builder.h
// Copyright: (C) 2001 British Telecommunications plc.
#ifndef clsfy_rbf_svm_smo_1_builder_h_
#define clsfy_rbf_svm_smo_1_builder_h_
//:
// \file
// \brief Describe an interface to an SMO SVM builder and additional logic
// \author Ian Scott
// \date Dec 2001

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_builder_base.h>
#include <clsfy/clsfy_rbf_svm.h>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_data_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Base for classes to build clsfy_classifier_base objects
class clsfy_rbf_svm_smo_1_builder : public clsfy_builder_base
{
  //: C, the upper bound on the Lagrange multipliers.
  // 0 means no (or infinite) upper bound.
  double boundC_;

  //: Width of Gaussian function
  double rbf_width_;

 public:

  //: Dflt ctor
   clsfy_rbf_svm_smo_1_builder(): boundC_(0), rbf_width_(1.0) {}

  //: Create empty model
  clsfy_classifier_base* new_classifier() const override{return new clsfy_rbf_svm();}

  //: Build model from data
  // returns the empirical error, or +INF if there is an error.
  // nclasses should be 1. The members of outputs() should be 0 or 1.
  double build(clsfy_classifier_base& classifier,
               mbl_data_wrapper<vnl_vector<double> >& inputs,
               unsigned nClasses,
               const std::vector<unsigned> &outputs) const override;

  //: Build a classifier from data.
  // returns the empirical error rate, or +INF if there is an error.
  // The members of outputs() should be 0 or 1.
  double build(clsfy_classifier_base& classifier,
               mbl_data_wrapper<vnl_vector<double> >& inputs,
               const std::vector<unsigned> &outputs) const;

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
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Return the class's IO version number
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  clsfy_builder_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Config from a stream.
  void config(std::istream&) override;
};

#endif // clsfy_rbf_svm_smo_1_builder_h_
