#ifndef clsfy_binary_threshold_1d_h_
#define clsfy_binary_threshold_1d_h_
//:
// \file
// \brief Simplest possible 1D classifier: A single thresholding function
// \author Tim Cootes

#include <clsfy/clsfy_classifier_1d.h>
#include <vnl/vnl_vector.h>


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
  virtual void class_probabilities(vcl_vector<double> &outputs, double input) const;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  virtual unsigned classify(double input) const
    { if (s_*input<threshold_) return 0; else return 1; }

  //: Log likelihood of being in the positive class/
  // Class probability = 1 / (1+exp(-log_l))
  virtual double log_l(double input) const;

  //: Set the threshold and orientation.
  void set(double s, double t)
  { s_=s; threshold_=t; }

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  virtual unsigned  n_classes() const { return 1;}

  //: Return parameters defining classifier in a vector (format depends on classifier)
  virtual vnl_vector<double> params() const;

  //: Set parameters defining classifier with a vector (format depends on classifier)
  virtual void set_params(const vnl_vector<double>& p);

  //: Equality operator for 1d classifiers
  virtual bool operator==(const clsfy_classifier_1d& x) const;

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
  virtual clsfy_classifier_1d* clone() const
  { return new clsfy_binary_threshold_1d(*this); }

  //: Load the class from a Binary File Stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // clsfy_binary_threshold_1d_h_
