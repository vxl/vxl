// This is mul/clsfy/clsfy_k_nearest_neighbour.h
// Copyright: (C) 2000 British Telecommunications plc.
#ifndef clsfy_k_nearest_neighbour_h_
#define clsfy_k_nearest_neighbour_h_
//:
// \file
// \brief Describe an KNN classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//   2 May 2001 IMS Converted to VXL
// \endverbatim

#include <clsfy/clsfy_classifier_base.h>
#include <vcl_iosfwd.h>

//: A Binary k-Nearest Neighbour Neighbour classifier
class clsfy_k_nearest_neighbour : public clsfy_classifier_base
{
  //: The number of nearest neighbours to look for.
  unsigned k_;

  //: The set of training input values.  Size should equal number of outputs.
  vcl_vector<vnl_vector<double> > trainInputs_;

  //: The set of training output values.  Size should equal number of inputs.
  vcl_vector<unsigned> trainOutputs_;

 public:
  //: Construct a KNN classifier.
  clsfy_k_nearest_neighbour():k_(1) {}

  //: Return the classification of the given probe vector.
  virtual unsigned classify(const vnl_vector<double> &input) const;

  //: Provides a probability-like value that the input being in each class.
  // output(i) i<nClasses, contains the probability that the input is in class i
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: This value has properties of a Log likelihood of being in class (binary classifiers only)
  // class probability = exp(logL) / (1+exp(logL))
  virtual double log_l(const vnl_vector<double> &input) const;

  //: Set the training data.
  void set(const vcl_vector<vnl_vector<double> > &inputs, const vcl_vector<unsigned> &outputs);

  //: The number of nearest neighbours to look for.
  // The default value is 1.
  unsigned k() const {return k_;}

  //: Set the number of nearest neighbours to look for.
  // The default value is 1.
  void set_k(unsigned k) {k_ = k;}

  //: Return a reference to the training vectors.
  // Vectors are ordered similarly to training_classes()
  const vcl_vector<vnl_vector<double> >& training_vectors() const {return trainInputs_;}

  //: Return a reference to the training classes
  // Vectors are ordered similarly to training_vectors()
  const vcl_vector<unsigned >& training_classes() const {return trainOutputs_;}

  //: The number of possible output classes.
  virtual unsigned n_classes() const {return 1;}

  //: The dimensionality of input vectors.
  virtual unsigned n_dims() const;

  //: Storage version number
  virtual short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_classifier_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // clsfy_k_nearest_neighbour_h_
