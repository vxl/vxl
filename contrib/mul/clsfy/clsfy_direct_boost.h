// This is mul/clsfy/clsfy_direct_boost.h
#ifndef clsfy_direct_boost_h_
#define clsfy_direct_boost_h_
//:
// \file
// \brief Classifier using adaboost on combinations of simple 1D classifiers
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_classifier_1d.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Classifier using adaboost on combinations of simple 1D classifiers
//  Uses a weighted combination of 1D classifiers applied to the
//  elements of the input vector.
class clsfy_direct_boost : public clsfy_classifier_base
{
 protected:

  //: The classifiers in order
  std::vector<clsfy_classifier_1d*> classifier_1d_;

  //: Coefficients applied to each classifier
  std::vector<double> wts_;

  //: Index of input vector appropriate for each classifier
  std::vector<int> index_;

  //: Thresholds given variable number of weak classifiers.
  // ie threshes[nc-1] is the threshold when using nc weak classifiers
  std::vector<double> threshes_;

  //: number of classifiers used
  int n_clfrs_used_;

  //: dimensionality of data.
  // (ie size of input vectors v, ie the total number of different features)
  int n_dims_;

//================protected methods =================================

  //: Delete objects on heap
  void delete_stuff();

 public:

  //: Default constructor
  clsfy_direct_boost();

  //: Copy constructor
  clsfy_direct_boost(const clsfy_direct_boost&);

  //: Copy operator
  clsfy_direct_boost& operator=(const clsfy_direct_boost&);

  //: Destructor
  ~clsfy_direct_boost() override;

  //: Comparison
  bool operator==(const clsfy_direct_boost& x) const;

  //: Clear all wts and classifiers
  void clear();

  //: Add classifier and alpha value
  void add_one_classifier(clsfy_classifier_1d* c1d, double wt, int index);

  //: Set number of classifiers used (when applying strong classifier)
  void set_n_clfrs_used(unsigned int x) {if (x <= wts_.size()) n_clfrs_used_ = x;}

  //: Access
  int n_clfrs_used() const {return n_clfrs_used_; }

   //: Add one threshold
  void add_one_threshold(double thresh);

  //: Add final threshold
  void add_final_threshold(double thresh);

  //: Find the posterior probability of the input being in the positive class.
  // The result is outputs(0)
  void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override;

  //: Classify the input vector.
  // Returns a number between 0 and nClasses-1 inclusive to represent the most likely class
  unsigned classify(const vnl_vector<double> &input) const override;

  //: Log likelihood of being in the positive class.
  // Class probability = 1 / (1+exp(-log_l))
  double log_l(const vnl_vector<double> &input) const override;

  //: The dimensionality of input vectors.
  unsigned n_dims() const override { return n_dims_;}

   //: Set number of classifiers used (when applying strong classifier)
  void set_n_dims(unsigned x) {n_dims_ = x;}

  //: The number of possible output classes.
  // 1 indicates a binary classifier
  unsigned n_classes() const override { return 1;}

  //: Set parameters.  Clones taken of *classifier[i]
  void set_parameters(const std::vector<clsfy_classifier_1d*>& classifier,
                      const std::vector<double>& threshes,
                      const std::vector<double>& wts,
                      const std::vector<int>& index);

  //: Access functions
  const std::vector<clsfy_classifier_1d*>& classifiers() const
    {return classifier_1d_;}

  const std::vector<double>& wts() const {return wts_;}

  const std::vector<int>& index() const {return index_;}

  const std::vector<double>& threshes() const
    {return threshes_;}

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
  { return new clsfy_direct_boost(*this); }

  //: Load the class from a Binary File Stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // clsfy_direct_boost_h_
