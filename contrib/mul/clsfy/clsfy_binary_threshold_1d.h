// This is mul/clsfy/clsfy_binary_threshold_1d.h
#ifndef clsfy_binary_threshold_1d_h_
#define clsfy_binary_threshold_1d_h_
//:
// \file
// \brief Simplest possible 1D classifier: A single thresholding function
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_classifier_1d.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Simplest possible 1D classifier: A single thresholding function.
//  Returns class zero if s_*x<threshold_

class clsfy_binary_threshold_1d : public clsfy_classifier_1d
{
 protected:
  double s_;
  double threshold_;
 public:

  //: Find the posterior probability of the input being in the positive class.
  // The result is outputs(0)
  void class_probabilities(std::vector<double> &outputs, double input) const override;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  unsigned classify(double input) const override
    { if (s_*input<threshold_) return 0; else return 1; }

  //: Log likelihood of being in the positive class/
  // Class probability = 1 / (1+exp(-log_l))
  double log_l(double input) const override;

  //: Set the threshold and orientation.
  void set(double s, double t)
  { s_=s; threshold_=t; }

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  unsigned  n_classes() const override { return 1;}

  //: Return parameters defining classifier in a vector (format depends on classifier)
  vnl_vector<double> params() const override;

  //: Set parameters defining classifier with a vector (format depends on classifier)
  void set_params(const vnl_vector<double>& p) override;

  //: Equality operator for 1d classifiers
  bool operator==(const clsfy_classifier_1d& x) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to a binary File Stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Create a deep copy.
  // Client is responsible for deleting returned object.
  clsfy_classifier_1d* clone() const override
  { return new clsfy_binary_threshold_1d(*this); }

  //: Load the class from a Binary File Stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // clsfy_binary_threshold_1d_h_
