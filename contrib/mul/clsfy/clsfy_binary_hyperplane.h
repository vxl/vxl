// This is mul/clsfy/clsfy_binary_hyperplane.h
// Copyright: (C) 2000 British Telecommunications PLC
#ifndef clsfy_binary_hyperplane_bin_h_
#define clsfy_binary_hyperplane_bin_h_
//:
// \file
// \brief Describe a linear binary classifier
// \author Ian Scott
// \date 4 June 2001

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_classifier_base.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:  A binary output hyperplane classifier.
// The classifier could be considered as a single perceptron.

class clsfy_binary_hyperplane : public clsfy_classifier_base
{
 protected:
  //: Hyperplane normal
  vnl_vector<double> weights_;
  //: Bias: ||weights_|| * distance from origin to hyperplane
  double bias_;

 public:

  //: Find the posterior probability of the input being in the positive class.
  // The result is outputs(0)
  void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  unsigned classify(const vnl_vector<double> &input) const override;

  //: Log likelihood of being in the positive class.
  // Class probability = 1 / (1+exp(-log_l))
  double log_l(const vnl_vector<double> &input) const override;

  //: Set the position of the hyperplane.
  // The object will take and maintain its own deep copy of the data.
  // This method should only be used by builders.
  void set(const vnl_vector<double> &weights, double bias)
  { weights_ = weights; bias_ = bias;}

  //: Get the classifier weight vector (= hyperplane normal)
  const vnl_vector<double> &weights() const { return weights_; }

  //: Get the classifier bias.
  double bias() const { return bias_; }

  //: The dimensionality of input vectors.
  unsigned n_dims() const override { return weights_.size();}

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  unsigned n_classes() const override { return 1;}

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
  clsfy_classifier_base* clone() const override
  { return new clsfy_binary_hyperplane(*this); }

  //: Load the class from a Binary File Stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // clsfy_binary_hyperplane_bin_h_
