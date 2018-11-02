// This is mul/clsfy/clsfy_k_nearest_neighbour.h
// Copyright: (C) 2000 British Telecommunications plc.
#ifndef clsfy_k_nearest_neighbour_h_
#define clsfy_k_nearest_neighbour_h_
//:
// \file
// \brief Describe a KNN classifier
// \author Ian Scott
// \date 2000-05-10
// \verbatim
//  Modifications
//   2 May 2001 IMS Converted to VXL
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_classifier_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A Binary k-Nearest Neighbour classifier
class clsfy_k_nearest_neighbour : public clsfy_classifier_base
{
  //: The number of nearest neighbours to look for.
  unsigned k_;

  //: The set of training input values.  Size should equal number of outputs.
  std::vector<vnl_vector<double> > trainInputs_;

  //: The set of training output values.  Size should equal number of inputs.
  std::vector<unsigned> trainOutputs_;

 public:
  //: Construct a KNN classifier.
  clsfy_k_nearest_neighbour():k_(1) {}

  //: Return the classification of the given probe vector.
  unsigned classify(const vnl_vector<double> &input) const override;

  //: Provides a probability-like value that the input being in each class.
  // output(i) i<nClasses, contains the probability that the input is in class i
  void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override;

  //: This value has properties of a Log likelihood of being in class (binary classifiers only)
  // class probability = exp(logL) / (1+exp(logL))
  double log_l(const vnl_vector<double> &input) const override;

  //: Set the training data.
  void set(const std::vector<vnl_vector<double> > &inputs, const std::vector<unsigned> &outputs);

  //: The number of nearest neighbours to look for.
  // The default value is 1.
  unsigned k() const {return k_;}

  //: Set the number of nearest neighbours to look for.
  // The default value is 1.
  void set_k(unsigned k) {k_ = k;}

  //: Return a reference to the training vectors.
  // Vectors are ordered similarly to training_classes()
  const std::vector<vnl_vector<double> >& training_vectors() const {return trainInputs_;}

  //: Return a reference to the training classes
  // Vectors are ordered similarly to training_vectors()
  const std::vector<unsigned >& training_classes() const {return trainOutputs_;}

  //: The number of possible output classes.
  unsigned n_classes() const override {return 1;}

  //: The dimensionality of input vectors.
  unsigned n_dims() const override;

  //: Storage version number
  virtual short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  clsfy_classifier_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // clsfy_k_nearest_neighbour_h_
