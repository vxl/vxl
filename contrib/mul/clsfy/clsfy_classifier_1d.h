// This is mul/clsfy/clsfy_classifier_1d.h
#ifndef clsfy_classifier_1d_h_
#define clsfy_classifier_1d_h_
//:
// \file
// \brief Describe an abstract classifier of 1D data
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_data_wrapper.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:  A common interface for 1-out-of-N classifiers of 1D data
// This class takes a scalar and classifies into one of N classes.
//
// Derived classes with binary in the name indicates that
// the classifier works with only two classes, 0 and 1.

class clsfy_classifier_1d
{
 public:

  // Dflt constructor
  clsfy_classifier_1d();

  // Destructor
  virtual ~clsfy_classifier_1d();
  // this is virtual in case it is referenced via a base class ptr e.g.  "delete ptr"

  //: Classify the input vector
  // returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  virtual unsigned classify(double input) const;

  //: Return parameters defining classifier in a vector (format depends on classifier)
  virtual vnl_vector<double> params() const = 0;

  //: Set parameters defining classifier with a vector (format depends on classifier)
  virtual void set_params(const vnl_vector<double>& p)=0;

  //: Return the probability the input being in each class.
  // output(i) 0<=i<n_classes, contains the probability that the input is in class i
  virtual void class_probabilities(std::vector<double> &outputs, double input) const = 0;

  //: Classify many input vectors
  virtual void classify_many(std::vector<unsigned> &outputs, mbl_data_wrapper<double> &inputs) const;

  //: Log likelihood of being in class (binary classifiers only)
  // class probability = 1 / (1+exp(-log_l))
  // Operation of this method is undefined for multiclass classifiers
  virtual double log_l(double input) const = 0;

  //: The number of possible output classes. If ==1, then it's a binary classifier.
  virtual unsigned  n_classes() const = 0;

  //: Equality operator for 1d classifiers
  virtual bool operator==(const clsfy_classifier_1d& x) const = 0;

  //: Name of the class
  virtual std::string is_a() const;

  //: Name of the class
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_classifier_1d* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_classifier_1d& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_classifier_1d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_classifier_1d& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os, const clsfy_classifier_1d& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os, const clsfy_classifier_1d* b);

//: Stream output operator for class reference
inline void vsl_print_summary(std::ostream& os, const clsfy_classifier_1d& b)
{ os << b;}

//: Stream output operator for class pointer
inline void vsl_print_summary(std::ostream& os, const clsfy_classifier_1d* b)
{ os << b;}


//----------------------------------------------------------

//: Calculate the fraction of test samples which are classified incorrectly
double clsfy_test_error(const clsfy_classifier_1d &classifier,
                        mbl_data_wrapper<double> & test_inputs,
                        const std::vector<unsigned> & test_outputs);

#endif // clsfy_classifier_1d_h_
