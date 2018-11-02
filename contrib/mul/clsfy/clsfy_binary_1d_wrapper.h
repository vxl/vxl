// Copyright: (C) 2009 Imorphics PLC
#ifndef clsfy_binary_1d_wrapper_h_
#define clsfy_binary_1d_wrapper_h_
//:
// \file
// \brief Wrap a classifier_1d in general classifier_base derivative.
// \author Ian Scott
// \date 2 Sep 2009

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_cloneable_ptr.h>
#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_classifier_1d.h>

//: Make a clsfy_classifier_1d derivative look like a clsfy_classifierbase derivative.
class clsfy_binary_1d_wrapper : public clsfy_classifier_base
{
 protected:
  //: Underlying 1d classifier.
  mbl_cloneable_nzptr<clsfy_classifier_1d> classifier_1d_;

 public:

  clsfy_binary_1d_wrapper();

  //: Find the posterior probability of the input being in the positive class.
  // The result is outputs(0)
  void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override
  { return classifier_1d_->class_probabilities(outputs, input(0)); }

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  unsigned classify(const vnl_vector<double> &input) const override
  { return classifier_1d_->classify(input(0)); }

  //: Log likelihood of being in the positive class.
  // Class probability = 1 / (1+exp(-log_l))
  double log_l(const vnl_vector<double> &input) const override
  { return classifier_1d_->log_l(input(0)); }

  //: Set the underlying classifier.
  // The object will take and maintain its own deep copy of the data.
  // This method should only be used by builders.
  void set_classifier_1d(const clsfy_classifier_1d &classifier)
  { classifier_1d_ = classifier; }

  //: Get the underlying classifier.
  const clsfy_classifier_1d &classifier_1d() const
  { return *classifier_1d_; }

  //: The dimensionality of input vectors.
  unsigned n_dims() const override { return 1u; }

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  unsigned n_classes() const override { return 1u; }

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to a binary File Stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load the class from a Binary File Stream
  void b_read(vsl_b_istream& bfs) override;

  //: Create a deep copy.
  // Client is responsible for deleting returned object.
  clsfy_classifier_base* clone() const override
  { return new clsfy_binary_1d_wrapper(*this); }
};

#endif // clsfy_binary_1d_wrapper_h_
