// This is mul/clsfy/clsfy_binary_hyperplane.h
// Copyright: (C) 2000 British Telecommunications PLC
#ifndef clsfy_binary_hyperplane_bin_h_
#define clsfy_binary_hyperplane_bin_h_
//:
// \file
// \brief Describe a linear binary classifier
// \author Ian Scott
// \date 4 June 2001

#include <clsfy/clsfy_classifier_base.h>
#include <vnl/vnl_vector.h>
#include <vcl_iosfwd.h>

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
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  virtual unsigned classify(const vnl_vector<double> &input) const;

  //: Log likelihood of being in the positive class.
  // Class probability = 1 / (1+exp(-log_l))
  virtual double log_l(const vnl_vector<double> &input) const;

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
  virtual unsigned n_dims() const { return weights_.size();}

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  virtual unsigned n_classes() const { return 1;}

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to a binary File Stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Create a deep copy.
  // Client is responsible for deleting returned object.
  virtual clsfy_classifier_base* clone() const
  { return new clsfy_binary_hyperplane(*this); }

  //: Load the class from a Binary File Stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // clsfy_binary_hyperplane_bin_h_
