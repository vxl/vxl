#ifndef clsfy_simple_adaboost_h_
#define clsfy_simple_adaboost_h_

//:
// \file
// \brief Classifier using adaboost on combinations of simple 1D classifiers
// \author Tim Cootes


#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_classifier_1d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>


//: Classifier using adaboost on combinations of simple 1D classifiers
class clsfy_simple_adaboost : public clsfy_classifier_base
{
protected:
  vcl_vector<clsfy_classifier_1d*> classifier_1d_;

  //: Weights on each classifier
  vnl_vector<double> wts_;

  //: Threshold for classification (half sum of weights)
  double bias_;

  //: Element of vector to use with each classifier
  vcl_vector<int> index_;

  //: Number of dimensions of input vectors
  unsigned n_dims_;

  //: Delete objects on heap
  void delete_stuff();
public:
  //: Default constructor
  clsfy_simple_adaboost();

  //: Copy constructor
  clsfy_simple_adaboost(const clsfy_simple_adaboost&);

  //: Copy operator
  clsfy_simple_adaboost& operator=(const clsfy_simple_adaboost&);

  //: Destructor
  ~clsfy_simple_adaboost();

  //: Find the posterior probability of the input being in the positive class.
  // The result is outputs(0)
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  unsigned virtual classify(const vnl_vector<double> &input) const;

  //: Log likelyhood of being in the positive class.
  // Class probability = 1 / (1+exp(-log_l))
  virtual double log_l(const vnl_vector<double> &input) const;

  //: The dimensionality of input vectors.
  unsigned virtual n_dims() const { return n_dims_;}

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  unsigned virtual n_classes() const { return 1;}

  //: Set parameters.  Clones taken of *classifier[i]
  void set_parameters(const vcl_vector<clsfy_classifier_1d*>& classifier,
                      const vcl_vector<vnl_vector<double> >& params,
                      const vnl_vector<double>& wts,
                      const vcl_vector<int>& index, int n_dims);

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
  { return new clsfy_simple_adaboost(*this); }

  //: Load the class from a Binary File Stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // clsfy_simple_adaboost_h_
