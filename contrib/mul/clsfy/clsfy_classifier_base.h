// This is mul/clsfy/clsfy_classifier_base.h
// Copyright: (C) 2000 British Telecommunications plc
#ifndef clsfy_classifier_base_h_
#define clsfy_classifier_base_h_
//:
// \file
// \brief Describe an abstract classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//  2 May 2001 IMS Converted to VXL
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_data_wrapper.h>
#include <vsl/vsl_binary_io.h>

//:  A common interface for 1-out-of-N classifiers
// This class takes a vector and classifies into one of
// N classes.
//
// Derived classes with binary in the name indicates that
// the classifier works with only two classes, 0 and 1.

class clsfy_classifier_base
{
 public:

  // Dflt constructor
   clsfy_classifier_base() {}

  // Destructor
   virtual ~clsfy_classifier_base() {}

  //: Classify the input vector
  // returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  virtual unsigned classify(const vnl_vector<double> &input) const;

  //: Return the probability the input being in each class.
  // output(i) 0<=i<nClasses, contains the probability that the input is in class i
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const = 0;

  //: Classify many input vectors
  virtual void classify_many(vcl_vector<unsigned> &outputs, mbl_data_wrapper<vnl_vector<double> > &inputs) const;

  //: Log likelihood of being in class (binary classifiers only)
  // class probability = 1 / (1+exp(-log_l))
  // Operation of this method is undefined for multiclass classifiers
  virtual double log_l(const vnl_vector<double> &input) const = 0;

  //: The number of possible output classes.
  virtual unsigned n_classes() const = 0;

  //: The dimensionality of input vectors.
  virtual unsigned n_dims() const = 0;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_classifier_base* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_classifier_base& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_classifier_base& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_classifier_base& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os, const clsfy_classifier_base& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os, const clsfy_classifier_base* b);

//: Stream output operator for class reference
inline void vsl_print_summary(vcl_ostream& os, const clsfy_classifier_base& b)
{ os << b;}

//: Stream output operator for class pointer
inline void vsl_print_summary(vcl_ostream& os, const clsfy_classifier_base* b)
{ os << b;}


//----------------------------------------------------------

//: Calculate the fraction of test samples which are classified incorrectly
double clsfy_test_error(const clsfy_classifier_base &classifier,
                        mbl_data_wrapper<vnl_vector<double> > & test_inputs,
                        const vcl_vector<unsigned> & test_outputs);

//: Calculate the fraction of test samples of a particular class which are classified incorrectly
// \return -1 if there are no samples of test_class. 
double clsfy_test_error(const clsfy_classifier_base &classifier,
                        mbl_data_wrapper<vnl_vector<double> > & test_inputs,
                        const vcl_vector<unsigned> & test_outputs,
                        unsigned test_class);


#endif // clsfy_classifier_base_h_
