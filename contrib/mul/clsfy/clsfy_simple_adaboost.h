// This is mul/clsfy/clsfy_simple_adaboost.h
#ifndef clsfy_simple_adaboost_h_
#define clsfy_simple_adaboost_h_
//:
// \file
// \brief Classifier using adaboost on combinations of simple 1D classifiers
// \author Tim Cootes

#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_classifier_1d.h>
#include <vnl/vnl_vector.h>
#include <vcl_iosfwd.h>

//: Classifier using adaboost on combinations of simple 1D classifiers
//  Uses a weighted combination of 1D classifiers applied to the
//  elements of the input vector.
class clsfy_simple_adaboost : public clsfy_classifier_base
{
 protected:

  //: The classifiers in order
  vcl_vector<clsfy_classifier_1d*> classifier_1d_;

  //: Coefficients applied to each classifier
  vcl_vector<double> alphas_;

  //: Index of input vector appropriate for each classifier
  vcl_vector<int> index_;

  //: number of classifiers used
  int n_clfrs_used_;

  //: dimensionality of data
  // (i.e. size of input vectors v, ie the total number of different features)
  int n_dims_;

//================protected methods =================================

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

  //: Comparison
  bool operator==(const clsfy_simple_adaboost& x) const;

  //: Clear all alphas and classifiers
  void clear();

  //: Add classifier and alpha value
  void add_classifier(clsfy_classifier_1d* c1d, double alpha, int index);

  //: Set number of classifiers used (when applying strong classifier)
  void set_n_clfrs_used(unsigned int x) {if (x <= alphas_.size()) n_clfrs_used_ = x;}

  //: Access
  int n_clfrs_used() const {return n_clfrs_used_; }

  //: Find the posterior probability of the input being in the positive class.
  // The result is outputs(0)
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  virtual unsigned classify(const vnl_vector<double> &input) const;

  //: Log likelihood of being in the positive class.
  // Class probability = 1 / (1+exp(-log_l))
  virtual double log_l(const vnl_vector<double> &input) const;

  //: The dimensionality of input vectors.
  virtual unsigned n_dims() const { return n_dims_;}

   //: Set number of classifiers used (when applying strong classifier)
  void set_n_dims(unsigned x) {n_dims_ = x;}

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  virtual unsigned n_classes() const { return 1;}

  //: Set parameters.  Clones taken of *classifier[i]
  void set_parameters(const vcl_vector<clsfy_classifier_1d*>& classifier,
                      const vcl_vector<double>& alphas,
                      const vcl_vector<int>& index);

  //: Access functions
  const vcl_vector<clsfy_classifier_1d*>& classifiers() const
    {return classifier_1d_;}

  const vcl_vector<double>& alphas() const
    {return alphas_;}

  const vcl_vector<int>& index() const
    {return index_;}

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
