// This is mul/clsfy/clsfy_rbf_parzen.h
// Copyright: (C) 2000 British Telecommunications plc.
#ifndef clsfy_rbf_parzen_h_
#define clsfy_rbf_parzen_h_
//:
// \file
// \brief Describe a Parzen window classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//   2 May 2001 IMS Converted to VXL
// \endverbatim

#include <clsfy/clsfy_classifier_base.h>
#include <vcl_iosfwd.h>

//: A Parzen binary classifier using an RBF Window
// A given vector, x0, is classified, by placing a window defined by w = exp(-1/(2*sigma^p) * |x-x0|^p),
// and finding the majority prediction of the nearby training vectors, each weighted by the window function.
class clsfy_rbf_parzen : public clsfy_classifier_base
{
  //: Workspace value which is cached for speed.
  // Inversely proportional to width of the RBF window function.  Default value is -0.5.
  //
  // width_ = 1/sqrt(-2*gamma_)
  double gamma_;

  //: Width of the rbf window function.
  // Default value is 1.0.
  // width_ = 1/sqrt(-2*gamma_)
  double width_;

  //: The power, p, in the window function. Default value is 2.0.
  double power_;

  //: The set of training input values.  Size should equal number of outputs.
  vcl_vector<vnl_vector<double> > trainInputs_;

  //: The set of training output values.  Size should equal number of inputs.
  vcl_vector<unsigned> trainOutputs_;

 public:
  //: Construct a Parzen classifier.
  clsfy_rbf_parzen() { set_rbf_width(1.0); set_power(2.0);}

  //: Return the classification of the given probe vector.
  virtual unsigned classify(const vnl_vector<double> &input) const;

  //: Provides a probability-like value that the input being in each class.
  // output(i) i<nClasses, contains the probability that the input is in class i
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Return the number of proximate training vectors, weighted by the windowing function.
  double weightings(const vnl_vector<double> &input) const;

  //: This value has properties of a Log likelihood of being in class (binary classifiers only)
  // class probability = exp(logL) / (1+exp(logL))
  virtual double log_l(const vnl_vector<double> &input) const;

  //: Set the training data.
  void set(const vcl_vector<vnl_vector<double> > &inputs, const vcl_vector<unsigned> &outputs);

  //: The 1st standard deviation width of the RBF window.
  // The default value is 1.
  double rbf_width() const { return width_;}

  //: Set the 1st standard deviation width of the RBF window.
  // The default value in the constructor is 1.
  void set_rbf_width(double sigma);

  //: The value p in the window function $exp(-1/(2*sigma^p) * |x-y|^p)$.
  // The value p affects the kurtosis, or peakyness of the window. Towards 0 gives a more peaked central spike, and longer tail.
  // Toward +inf gives a broader peak, and shorter tail.
  // The default value is 2, giving a Gaussian distribution.
  double power() const { return power_;}

  //: The value p in the window function $exp(-1/(2*sigma^p) * |x-y|^p)$.
  // The value p affects the kurtosis, or peakyness of the window.
  // Towards 0 gives a more peaked central spike, and longer tail.
  // Toward +inf gives a broader peak, and shorter tail.
  // The default value is 2, giving a Gaussian distribution.
  void set_power(double p);

  //: Return a reference to the training vectors.
  // Vectors are ordered similarly to training_classes()
  const vcl_vector<vnl_vector<double> > & training_vectors() const
  {return trainInputs_;}

  //: Return a reference to the training classes
  // Vectors are ordered similarly to training_vectors()
  const vcl_vector<unsigned > & training_classes() const
  {return trainOutputs_;}

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

#endif // clsfy_rbf_parzen_h_
